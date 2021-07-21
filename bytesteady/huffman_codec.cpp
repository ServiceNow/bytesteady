/*
 * Copyright 2021 ServiceNow
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bytesteady/huffman_codec.hpp"

#include <limits>
#include <string>

#include "bytesteady/bit_array.hpp"
#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"
#include "thunder/serializer/binary_protocol.hpp"
#include "thunder/serializer/serializer.hpp"
#include "thunder/serializer/static.hpp"
#include "thunder/serializer/text_protocol.hpp"

#include "bytesteady/huffman_codec-inl.hpp"

#include "thunder/serializer/binary_protocol-inl.hpp"
#include "thunder/serializer/serializer-inl.hpp"
#include "thunder/serializer/static-inl.hpp"
#include "thunder/serializer/text_protocol-inl.hpp"

namespace bytesteady {

bool HuffmanCodec::Node::operator>(const Node &b) const {
  return value > b.value;
}

HuffmanCodec::HuffmanCodec(const size_array &g) : gram_(g) {};

void HuffmanCodec::build(const data_callback &callback) {
  buildFrequencyFromData(callback, &frequency_);
  buildTreeFromFrequency(frequency_, &tree_);
  buildTableFromTree(tree_, &table_);
}

void HuffmanCodec::encode(
    const byte_array &input, byte_array *output) const {
  // Find the minimum and maximum gram
  size_type min_gram = ::std::numeric_limits< size_type >::max();
  size_type max_gram = ::std::numeric_limits< size_type >::min();
  for (const size_type g : gram_) {
    if (min_gram > g) {
      min_gram = g;
    }
    if (max_gram < g) {
      max_gram = g;
    }
  }

  if (min_gram == max_gram) {
    encodeSingleGramLength(input, output, min_gram);
  } else {
    encodeMultiGramLength(input, output, min_gram, max_gram);
  }
}

void HuffmanCodec::encodeSingleGramLength(
    const byte_array &input, byte_array *output, size_type gram_length) const {
  // Get the pad bit
  uint8_t pad_bit = table_.at(::std::string())[0];

  BitArray bits;
  size_type i = 0;
  while (i < input.size()) {
    if (i + gram_length <= input.size()) {
      ::std::string key(
           reinterpret_cast< const char * >(&input[i]), gram_length);
      if (table_.find(key) != table_.end()) {
        const byte_array &gram_bits = table_.at(key);
        for (const uint8_t &bit : gram_bits) {
          bits.pushBack(bit);
        }
        i = i + gram_length;
      } else {
        bits.pushBack(pad_bit);
        i = i + 1;
      }
    } else {
      bits.pushBack(pad_bit);
      i = i + 1;
    }
  }
  
  bits.getPaddedBytes(pad_bit, output);
}

void HuffmanCodec::encodeMultiGramLength(
    const byte_array &input, byte_array *output, size_type min_gram,
    size_type max_gram) const {
  // Get the pad_bit
  uint8_t pad_bit = table_.at(::std::string())[0];
  
  // bits[j] is the bit array for input byte up to location (i - max_gram + j)
  ::std::deque< BitArray > bits(max_gram);
  // Loop over bytes
  for (size_type i = min_gram - 1; i < input.size(); ++i) {
    size_type best_gram = 0;
    size_type best_length = ::std::numeric_limits< size_type >::max();
    for (size_type g : gram_) {
      // Can seek to the i - g + 1 byte
      if (g <= i + 1) {
        ::std::string key(
             reinterpret_cast< const char * >(&input[i + 1 - g]), g);
        if (table_.find(key) != table_.end()) {
          // Found the key
          size_type gram_code_length = bits[max_gram - g].size() +
              table_.at(key).size();
          if (best_length > gram_code_length) {
            best_length = gram_code_length;
            best_gram = g;
          }
        }
      }
    }
    if (best_gram > 0) {
      ::std::string key(
           reinterpret_cast< const char * >(&input[i + 1 - best_gram]),
           best_gram);
      const byte_array &gram_bits = table_.at(key);
      BitArray current_bits(bits[max_gram - best_gram]);
      for (const uint8_t &bit : gram_bits) {
        current_bits.pushBack(bit);
      }
      bits.pop_front();
      bits.push_back(current_bits);
    } else {
      // If no best_gram is found, use the pad_bit to encode the current byte
      BitArray current_bits(bits[bits.size() - 1]);
      current_bits.pushBack(pad_bit);
      bits.pop_front();
      bits.push_back(current_bits);
    }
  }

  bits[bits.size() - 1].getPaddedBytes(pad_bit, output);
}

void HuffmanCodec::decode(
    const byte_array &input, byte_array *output) const {
  output->clear();
  const Node *current = &tree_;
  for (const uint8_t &byte : input) {
    uint8_t base = 0b10000000;
    for (size_type i = 0; i < 8; ++i) {
      uint8_t bit = (byte & base) > 0 ? 1 : 0;
      base = base >> 1;
      if (bit == 0) {
        current = current->left.get();
      } else {
        current = current->right.get();
      }
      if (current->left == nullptr && current->right == nullptr) {
        for (const char &byte : current->key) {
          output->push_back(static_cast< uint8_t >(byte));
        }
        current = &tree_;
      }
    }
  }
}

void HuffmanCodec::buildFrequencyFromData(
    const data_callback &callback, value_table *frequency) {
  // Start the loop
  size_type total = 0;
  size_table count;
  byte_array input;
  while (callback(&input) == true) {
    for (const size_type &g : gram_) {
      for (size_type j = 0; input.size() >= g &&
               j < input.size() - g + 1; j = j + g) {
        ::std::string key(reinterpret_cast < const char * >(&input[j]), g);
        if (count.find(key) == count.end()) {
          count[key] = 0;
        }
        count[key] = count[key] + 1;
        total = total + 1;
      }
    }
  }

  // Fill the frequency table
  frequency->clear();
  for (const typename size_table::value_type &pair : count) {
    (*frequency)[pair.first] = static_cast< value_type >(pair.second) /
        static_cast< value_type >(total);
  }
  // Unknown string will be given 1-bit code, and also used for padding
  (*frequency)[::std::string()] = 2.0;
}

void HuffmanCodec::buildTreeFromFrequency(
    const value_table &frequency, Node *tree) {
  node_queue queue;
  for (const typename value_table::value_type &pair : frequency) {
    queue.push(Node{pair.first, pair.second, nullptr, nullptr});
  }
  while (queue.size() > 1) {
    // Left has larger frequency value than right, giving code 0
    Node right = queue.top();
    queue.pop();
    Node left = queue.top();
    queue.pop();
    queue.push(Node{::std::string(), left.value + right.value,
                    ::std::make_shared< Node >(left),
                    ::std::make_shared< Node >(right)});
  }
  *tree = queue.top();
}

void HuffmanCodec::buildTableFromTree(const Node &tree, byte_table *table) {
  table->clear();
  buildTableFromTreeRecursion(byte_array(), tree, table);
}

void HuffmanCodec::buildTreeFromTable(
    const value_table &frequency, const byte_table &table, Node *tree) {
  tree->key.clear();
  tree->value = 0.0;
  tree->left = nullptr;
  tree->right = nullptr;
  for (const typename byte_table::value_type &pair : table) {
    Node *current = tree;
    value_type frequency_value = frequency.at(pair.first);
    for (const uint8_t &byte : pair.second) {
      current->value = current->value + frequency_value;
      if (byte == 0) {
        current->left = current->left == nullptr ? ::std::make_shared< Node >(
            Node{::std::string(), 0.0, nullptr, nullptr}) : current->left;
        current = current->left.get();
      } else {
        current->right = current->right == nullptr ?
            ::std::make_shared< Node >(Node{
                ::std::string(), 0.0, nullptr, nullptr}) : current->right;
        current = current->right.get();
      }
    }
    current->key = pair.first;
    current->value = frequency_value;
  }
}

void HuffmanCodec::buildTableFromTreeRecursion(
    const byte_array &prefix, const Node &tree, byte_table *table) {
  if (tree.left == nullptr && tree.right == nullptr) {
    (*table)[tree.key] = prefix;
  } else {
    byte_array left_prefix(prefix);
    left_prefix.push_back(0);
    buildTableFromTreeRecursion(left_prefix, *(tree.left), table);
    byte_array right_prefix(prefix);
    right_prefix.push_back(1);
    buildTableFromTreeRecursion(right_prefix, *(tree.right), table);
  }
}

const typename HuffmanCodec::size_array &HuffmanCodec::gram() const {
  return gram_;
}

void HuffmanCodec::set_gram(const size_array &g) {
  gram_ = g;
}

const typename HuffmanCodec::value_table &HuffmanCodec::frequency() const {
  return frequency_;
}

const typename HuffmanCodec::byte_table &HuffmanCodec::table() const {
  return table_;
}

void HuffmanCodec::set(const value_table &frequency, const byte_table &table) {
  frequency_ = frequency;
  table_ = table;
  buildTreeFromTable(frequency, table, &tree_);
}

} //  namespace bytesteady

namespace thunder {
namespace serializer {

template void save(
    StringBinarySerializer *s, const ::bytesteady::HuffmanCodec &c);
template void StringBinarySerializer::save(
    const ::bytesteady::HuffmanCodec &c);
template void load(
    StringBinarySerializer *s, ::bytesteady::HuffmanCodec *c);
template void StringBinarySerializer::load(
    ::bytesteady::HuffmanCodec *c);
template void save(
    FileBinarySerializer *s, const ::bytesteady::HuffmanCodec &c);
template void FileBinarySerializer::save(
    const ::bytesteady::HuffmanCodec &c);
template void load(
    FileBinarySerializer *s, ::bytesteady::HuffmanCodec *c);
template void FileBinarySerializer::load(
    ::bytesteady::HuffmanCodec *c);
template void save(
    StringTextSerializer *s, const ::bytesteady::HuffmanCodec &c);
template void StringTextSerializer::save(
    const ::bytesteady::HuffmanCodec &c);
template void load(
    StringTextSerializer *s, ::bytesteady::HuffmanCodec *c);
template void StringTextSerializer::load(
    ::bytesteady::HuffmanCodec *c);
template void save(
    FileTextSerializer *s, const ::bytesteady::HuffmanCodec &c);
template void FileTextSerializer::save(
    const ::bytesteady::HuffmanCodec &c);
template void load(
    FileTextSerializer *s, ::bytesteady::HuffmanCodec *c);
template void FileTextSerializer::load(
    ::bytesteady::HuffmanCodec *c);

}  // namespace serializer
}  // namespace thunder
