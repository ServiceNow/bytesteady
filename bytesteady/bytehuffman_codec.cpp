/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/bytehuffman_codec.hpp"

#include <functional>
#include <string>

#include "bytesteady/integer.hpp"

#include "thunder/serializer.hpp"
#include "thunder/serializer/binary_protocol.hpp"
#include "thunder/serializer/serializer.hpp"
#include "thunder/serializer/static.hpp"
#include "thunder/serializer/text_protocol.hpp"

#include "bytesteady/bytehuffman_codec-inl.hpp"

#include "thunder/serializer/binary_protocol-inl.hpp"
#include "thunder/serializer/serializer-inl.hpp"
#include "thunder/serializer/static-inl.hpp"
#include "thunder/serializer/text_protocol-inl.hpp"

namespace bytesteady {

bool BytehuffmanCodec::Node::operator>(const Node &b) const {
  return value > b.value;
}

BytehuffmanCodec::BytehuffmanCodec(const size_array &g) :
    gram_(g.size() > 0 ? g[0] : 1) {};

void BytehuffmanCodec::build(const data_callback &callback) {
  buildFrequencyFromData(callback, &frequency_);
  buildTreeFromFrequency(frequency_, &tree_);
  buildCodeFromTree(tree_, &code_);
}

void BytehuffmanCodec::encode(
    const byte_array &input, byte_array *output) const {
  const byte_array &unrecognized_code = code_.at(::std::string());

  output->clear();
  bool unrecognized_gram = false;
  size_type i = 0;
  while (i < input.size()) {
    if (i + gram_ <= input.size()) {
      ::std::string key(reinterpret_cast< const char * >(&input[i]), gram_);
      if (code_.find(key) != code_.end()) {
        // Found gram
        if (unrecognized_gram == true) {
          for (const uint8_t &byte : unrecognized_code) {
            output->push_back(byte);
          }
        }
        for (const uint8_t &byte : code_.at(key)) {
          output->push_back(byte);
        }
        unrecognized_gram = false;
        i = i + gram_;
      } else {
        // Gram not found
        unrecognized_gram = true;
        i = i + 1;
      }
    } else {
      // Trailing gram
      for (const uint8_t &byte : unrecognized_code) {
        output->push_back(byte);
      }
      unrecognized_gram = false;
      i = input.size();
    }
  }
  // gram_ == 1 and unrecognized_gram was identified but not flushed
  if (unrecognized_gram == true) {
    for (const uint8_t &byte : unrecognized_code) {
      output->push_back(byte);
    }
  }
}

void BytehuffmanCodec::decode(
    const byte_array &input, byte_array *output) const {
  output->clear();
  const Node *current = &tree_;
  for (const uint8_t &input_byte : input) {
    current = &(current->children[static_cast< size_type >(input_byte)]);
    if (current->children.size() == 0) {
      for (const char &output_char : current->key) {
        output->push_back(static_cast< uint8_t >(output_char));
      }
      current = &tree_;
    }
  }
}

void BytehuffmanCodec::buildFrequencyFromData(
    const data_callback &callback, value_table *frequency) const {
  size_type total = 0;
  size_table count;
  byte_array input;
  while (callback(&input) == true) {
    for (size_type i = 0; i + gram_ <= input.size(); i = i + gram_) {
      ::std::string key(reinterpret_cast< const char * >(&input[i]), gram_);
      if (count.find(key) == count.end()) {
        count[key] = 1;
      } else {
        count[key] = count[key] + 1;
      }
      total = total + 1;
    }
  }

  frequency->clear();
  for (const typename size_table::value_type &pair : count) {
    (*frequency)[pair.first] = static_cast< value_type >(pair.second) /
        static_cast< value_type >(total);
  }
  // Unknown string will be given longest code
  (*frequency)[::std::string()] = 0.0;
}

void BytehuffmanCodec::buildTreeFromFrequency(
    const value_table &frequency, Node *tree) const {
  node_queue queue;
  for (const typename value_table::value_type &pair : frequency) {
    queue.push(Node{pair.first, pair.second});
  }
  // The first node is generated to contain (residual of 255) nodes
  if (queue.size() % 255 != 1) {
    Node node{::std::string(), 0.0};
    size_type residual = queue.size() % 255;
    for (size_type i = 0; i < residual; ++i) {
      Node child = queue.top();
      queue.pop();
      node.children.push_back(child);
      node.value = node.value + child.value;
    }
    queue.push(node);
  }
  // Rest of the node can be built into a tree by reducing 255 nodes at a time
  while (queue.size() > 1) {
    Node node{::std::string(), 0.0};
    for (size_type i = 0; i < 256 && queue.size() > 0; ++i) {
      Node child = queue.top();
      queue.pop();
      node.children.push_back(child);
      node.value = node.value + child.value;
    }
    queue.push(node);
  }

  *tree = queue.top();
}

void BytehuffmanCodec::buildCodeFromTree(
    const Node &tree, byte_array_table *code) const {
  ::std::function< void (const byte_array &, const Node &) > callback =
      [&](const byte_array &prefix, const Node &tree) -> void {
        if (tree.children.size() == 0) {
          (*code)[tree.key] = prefix;
        } else {
          for (size_type i = 0; i < tree.children.size(); ++i) {
            byte_array children_prefix(prefix);
            children_prefix.push_back(static_cast< uint8_t >(i));
            callback(children_prefix, tree.children[i]);
          }
        }
      };
  code->clear();
  callback(byte_array(), tree);
}

void BytehuffmanCodec::buildTreeFromCode(
    const value_table &frequency, const byte_array_table &code,
    Node *tree) const {
  tree->key.clear();
  tree->value = 0.0;
  tree->children.clear();

  for(const typename byte_array_table::value_type &pair : code) {
    Node *current = tree;
    value_type value = frequency.at(pair.first);
    for (const uint8_t &byte : pair.second) {
      size_type index = static_cast< size_type >(byte);
      current->value = current->value + value;
      if (index >= current->children.size()) {
        for (size_type i = current->children.size(); i <= index; ++i) {
          current->children.push_back(Node{::std::string(), 0.0});
        }
      }
      current = &(current->children[index]);
    }
    current->key = pair.first;
    current->value = value;
  }
}

typename BytehuffmanCodec::size_type BytehuffmanCodec::gram() const {
  return gram_;
}

void BytehuffmanCodec::set_gram(size_type g) {
  gram_ = g;
}

const BytehuffmanCodec::value_table &BytehuffmanCodec::frequency() const {
  return frequency_;
}

const BytehuffmanCodec::byte_array_table &BytehuffmanCodec::code() const {
  return code_;
}

void BytehuffmanCodec::set(const value_table &f, const byte_array_table &c) {
  frequency_ = f;
  code_ = c;
  buildTreeFromCode(frequency_, code_, &tree_);
}

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template void save(
    StringBinarySerializer *s, const ::bytesteady::BytehuffmanCodec &c);
template void StringBinarySerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
template void load(
    StringBinarySerializer *s, ::bytesteady::BytehuffmanCodec *c);
template void StringBinarySerializer::load(
    ::bytesteady::BytehuffmanCodec *c);
template void save(
    FileBinarySerializer *s, const ::bytesteady::BytehuffmanCodec &c);
template void FileBinarySerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
template void load(
    FileBinarySerializer *s, ::bytesteady::BytehuffmanCodec *c);
template void FileBinarySerializer::load(
    ::bytesteady::BytehuffmanCodec *c);
template void save(
    StringTextSerializer *s, const ::bytesteady::BytehuffmanCodec &c);
template void StringTextSerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
template void load(
    StringTextSerializer *s, ::bytesteady::BytehuffmanCodec *c);
template void StringTextSerializer::load(
    ::bytesteady::BytehuffmanCodec *c);
template void save(
    FileTextSerializer *s, const ::bytesteady::BytehuffmanCodec &c);
template void FileTextSerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
template void load(
    FileTextSerializer *s, ::bytesteady::BytehuffmanCodec *c);
template void FileTextSerializer::load(
    ::bytesteady::BytehuffmanCodec *c);

}  // namespace serializer
}  // namespace thunder
