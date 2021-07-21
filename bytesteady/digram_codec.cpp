/*
 * Copyright 2020 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/digram_codec.hpp"

#include <limits>
#include <string>
#include <utility>

#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"
#include "thunder/serializer/binary_protocol.hpp"
#include "thunder/serializer/serializer.hpp"
#include "thunder/serializer/static.hpp"
#include "thunder/serializer/text_protocol.hpp"

#include "bytesteady/digram_codec-inl.hpp"

#include "thunder/serializer/binary_protocol-inl.hpp"
#include "thunder/serializer/serializer-inl.hpp"
#include "thunder/serializer/static-inl.hpp"
#include "thunder/serializer/text_protocol-inl.hpp"

namespace bytesteady {

bool DigramCodec::Node::operator<(const Node &b) const {
  return value < b.value ? true : source.size() < b.source.size();
}

DigramCodec::DigramCodec(const size_array &g) {
  dict_size_ = g.size() > 0 ? g[0] : 1;
  gram_size_ = g.size() > 1 ? g[1] : ::std::numeric_limits< size_type >::max();
  count_size_ = g.size() > 2 ? g[2] :
      ::std::numeric_limits< typename size_table::size_type >::max();
  count_threshold_ = g.size() > 3 ? g[3] : 0;
}

void DigramCodec::build(const data_callback &callback) {
  // Build the count table
  buildCountFromData(callback, &count_);
  // Build the base gram set
  buildBaseGramSetFromCount(&count_, &base_gram_set_);
  // Build the gram text table from count table
  buildGramTextFromCount(&count_, &base_gram_set_, &gram_text_);
  // Build the replace list from the gram text table
  buildReplaceFromGramText(gram_text_, &replace_);
}

void DigramCodec::encode(const byte_array &input, byte_array *output) const {
  // It is okay to ignore training grams, but not chunk size in encoding
  ::std::string input_string;
  ::std::string output_string;
  for (size_type i = 0; i < input.size(); i = i + gram_size_) {
    input_string.assign(
        reinterpret_cast< const char * >(&input[i]),
        i + gram_size_ > input.size() ? (input.size() - i) : gram_size_);
    for (size_type j = 0; j < replace_.size(); ++j) {
      replaceString(replace_[j].first, replace_[j].second, &input_string);
    }
    output_string.append(input_string);
  }
  output->assign(reinterpret_cast< const uint8_t *>(output_string.c_str()),
                 reinterpret_cast< const uint8_t *>(
                     output_string.c_str() + output_string.size()));
}

void DigramCodec::decode(const byte_array &input, byte_array *output) const {
  ::std::string input_string(
       reinterpret_cast< const char * >(&input[0]), input.size());
  // It is okay to ignore chunk size and trailing grams in decoding
  for (size_type i = 0; i < replace_.size(); ++i) {
    replaceString(replace_[replace_.size() - 1 - i].second,
            replace_[replace_.size() - 1 - i].first,
            &input_string);
  }
  output->assign(reinterpret_cast< const uint8_t *>(input_string.c_str()),
                 reinterpret_cast< const uint8_t *>(
                     input_string.c_str() + input_string.size()));
}

void DigramCodec::buildCountFromData(
    const data_callback &callback, size_table *count) const {
  byte_array input;
  count->clear();
  while (callback(&input) == true) {
    // Build the n-gram count
    size_type input_size = input.size() / dict_size_ * dict_size_;
    // Operate in gram_size_ chunks
    for (size_type i = 0; i < input_size; i = i + gram_size_) {
      for (size_type j = i; j < i + gram_size_ && j < input_size; ++j) {
        for (size_type g = 1; g <= gram_size_ && j + g <= i + gram_size_ &&
                 j + g <= input_size; ++g) {
          ::std::string gram(reinterpret_cast< const char * >(&input[j]), g);
          if (count->find(gram) == count->end()) {
            (*count)[gram] = 1;
          } else {
            (*count)[gram] = (*count)[gram] + 1;
          }
        }
      }
    }
    // Reduce the n-gram count table size
    if (count->size() > count_size_) {
      typename size_table::iterator i = count->begin();
      while (i != count->end()) {
        if (i->second < count_threshold_) {
          i = count->erase(i);
        } else {
          ++i;
        }
      }
    }
  }
}

void DigramCodec::buildBaseGramSetFromCount(
    size_table *count, string_set *base_gram_set) const {
  base_gram_set->clear();
  byte_array base_gram_byte(dict_size_, 0);
  ::std::string base_gram;
  do {
    base_gram.assign(reinterpret_cast< const char * >(
        &base_gram_byte[0]), dict_size_);
    if (count->find(base_gram) != count->end()) {
      base_gram_set->insert(base_gram);
      // Remove the count in all sub-grams, except for itself
      for (size_type g = 1; g <= gram_size_ && g < base_gram.size(); ++g) {
        ::std::string sub_base_gram = base_gram.substr(0, g);
        if (count->find(sub_base_gram) != count->end()) {
          if ((*count)[base_gram] < (*count)[sub_base_gram]) {
            (*count)[sub_base_gram] = (*count)[sub_base_gram] -
                (*count)[base_gram];
          } else {
            count->erase(sub_base_gram);
          }
        }
      }
      for (size_type i = 1; i < base_gram.size(); ++i) {
        for (size_type g = 1; g <= gram_size_ && i + g <= base_gram.size();
             ++g) {
          ::std::string sub_base_gram = base_gram.substr(i, g);
          if (count->find(sub_base_gram) != count->end()) {
            if ((*count)[base_gram] < (*count)[sub_base_gram]) {
              (*count)[sub_base_gram] = (*count)[sub_base_gram] -
                  (*count)[base_gram];
            } else {
              count->erase(sub_base_gram);
            }
          }
        }
      }
    }
  } while (increase(&base_gram_byte) == 0);
}

void DigramCodec::buildGramTextFromCount(
    size_table *count, string_set *base_gram_set,
    string_table *gram_text) const {
  // Make gram_set contain all the entries in the base_gram_set
  gram_text->clear();
  string_set source_set;
  for (const ::std::string &gram : *base_gram_set) {
    (*gram_text)[gram] = gram;
    source_set.insert(gram);
  }

  bool found_source = false;
  bool found_target = false;
  do {
    // Find the next source
    node_queue digram_queue;
    for (const typename string_table::value_type &left_pair : *gram_text) {
      for (const typename string_table::value_type &right_pair : *gram_text) {
        ::std::string digram = left_pair.first + right_pair.first;
        ::std::string digram_text = left_pair.second + right_pair.second;
        if (count->find(digram_text) != count->end() &&
            source_set.find(digram_text) == source_set.end()) {
          // Use target to store the digram, and source to store the clear text
          digram_queue.push(Node{digram, digram_text, (*count)[digram_text]});
        }
      }
    }
    if (digram_queue.size() > 0) {
      found_source = true;
    } else {
      found_source = false;
    }

    // Find the next target
    byte_array target_byte(dict_size_, 0);
    ::std::string target(reinterpret_cast< const char * >(
         &target_byte[0]), dict_size_);
    while (gram_text->find(target) != gram_text->end() &&
           increase(&target_byte) == 0) {
      target.assign(reinterpret_cast< const char * >(
          &target_byte[0]), dict_size_);
    }
    if (gram_text->find(target) == gram_text->end()) {
      found_target = true;
    } else {
      found_target = false;
    }

    if (found_source && found_target) {
      ::std::string source = digram_queue.top().source;
      (*gram_text)[target] = source;
      source_set.insert(source);
      // Remove the count associated with source, assuming all text in the left
      // and in the right are cleared. Only grams that are at the boundaries
      // need to be discounted, excluding the source itself.
      ::std::string digram = digram_queue.top().target;
      ::std::string left_gram = digram.substr(0, digram.size() / 2);
      ::std::string right_gram = digram.substr(
           digram.size() / 2, digram.size() / 2);
      ::std::string left_text = (*gram_text)[left_gram];
      ::std::string right_text = (*gram_text)[right_gram];
      if (count->find(left_text) != count->end()) {
        if ((*count)[source] < (*count)[left_text]) {
          (*count)[left_text] = (*count)[left_text] - (*count)[source];
        } else {
          count->erase(left_text);
        }
      }
      if (count->find(right_text) != count->end()) {
        if ((*count)[source] < (*count)[right_text]) {
          (*count)[right_text] = (*count)[right_text] - (*count)[source];
        } else {
          count->erase(right_text);
        }
      }
      size_type right_begin = left_text.size();
      for (size_type g = right_begin + 1; g <= gram_size_ &&
               g < source.size(); ++g) {
        ::std::string sub_gram = source.substr(0, g);
        if (count->find(sub_gram) != count->end()) {
          if ((*count)[source] < (*count)[sub_gram]) {
            (*count)[sub_gram] = (*count)[sub_gram] - (*count)[source];
          } else {
              count->erase(sub_gram);
          }
        }
      }
      for (size_type i = 1; i < right_begin; ++i) {
        for (size_type g = right_begin - i + 1; g <= gram_size_ &&
                 i + g <= source.size(); ++g) {
          ::std::string sub_gram = source.substr(i, g);
          if (count->find(sub_gram) != count->end()) {
            if ((*count)[source] < (*count)[sub_gram]) {
              (*count)[sub_gram] = (*count)[sub_gram] - (*count)[source];
            } else {
              count->erase(sub_gram);
            }
          }
        }
      }
      // Check for discounted target in gram_text
      typename string_table::iterator i = gram_text->begin();
      while (i != gram_text->end()) {
        if (base_gram_set->find(i->second) == base_gram_set->end() &&
            count->find(i->second) == count->end()) {
          source_set.erase(i->second);
          i = gram_text->erase(i);
        } else {
          ++i;
        }
      }
    }
  } while (found_source && found_target);
}

void DigramCodec::buildReplaceFromGramText(
    const string_table &gram_text, replace_array *replace) const {
  // Build priority queue ranked by length of replace source
  node_queue queue;
  for (const typename string_table::value_type &pair : gram_text) {
    if (pair.first != pair.second) {
      queue.push(Node{pair.first, pair.second, 0});
    }
  }

  // Put the text to replace list
  replace->clear();
  while (!queue.empty()) {
    const Node &top = queue.top();
    replace->push_back(::std::make_pair(top.source, top.target));
    queue.pop();
  }
}

uint8_t DigramCodec::increase(byte_array *key) const {
  uint8_t carry = 1;
  size_type digit = 0;
  while (carry == 1 && digit < key->size()) {
    if ((*key)[key->size() - 1 - digit] == 255) {
      (*key)[key->size() - 1 - digit] = 0;
    } else {
      ++(*key)[key->size() - 1 - digit];
      carry = 0;
    }
    ++digit;
  }
  return carry;
}

void DigramCodec::replaceString(
    const ::std::string &source, const ::std::string &target,
    ::std::string *output) const {
  size_type position = 0;
  while((position = output->find(source, position)) != ::std::string::npos) {
    // The position must be somewhere on the dict_size_
    if (position - position / dict_size_ * dict_size_ == 0) {
      output->replace(position, source.length(), target);
      position = position + target.length();
    } else {
      ++position;
    }
  }
}

typename DigramCodec::size_type DigramCodec::dict_size() const {
  return dict_size_;
}

void DigramCodec::set_dict_size(size_type d) {
  dict_size_ = d;
}

typename DigramCodec::size_type DigramCodec::gram_size() const {
  return gram_size_;
}

void DigramCodec::set_gram_size(size_type g) {
  gram_size_ = g;
}

typename DigramCodec::size_type DigramCodec::count_size() const {
  return count_size_;
}

void DigramCodec::set_count_size(size_type c) {
  count_size_ = c;
}

typename DigramCodec::size_type DigramCodec::count_threshold() const {
  return count_threshold_;
}

void DigramCodec::set_count_threshold(size_type t) {
  count_threshold_ = t;
}

const typename DigramCodec::replace_array &DigramCodec::replace() const {
  return replace_;
}

void DigramCodec::set_replace(const replace_array &r) {
  replace_ = r;
}

const typename DigramCodec::size_table &DigramCodec::count() const {
  return count_;
}

const typename DigramCodec::string_set &DigramCodec::base_gram_set() const {
  return base_gram_set_;
}

const typename DigramCodec::string_table &DigramCodec::gram_text() const {
  return gram_text_;
}

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template void save(
    StringBinarySerializer *s, const ::bytesteady::DigramCodec &c);
template void StringBinarySerializer::save(
    const ::bytesteady::DigramCodec &c);
template void load(
    StringBinarySerializer *s, ::bytesteady::DigramCodec *c);
template void StringBinarySerializer::load(
    ::bytesteady::DigramCodec *c);
template void save(
    FileBinarySerializer *s, const ::bytesteady::DigramCodec &c);
template void FileBinarySerializer::save(
    const ::bytesteady::DigramCodec &c);
template void load(
    FileBinarySerializer *s, ::bytesteady::DigramCodec *c);
template void FileBinarySerializer::load(
    ::bytesteady::DigramCodec *c);
template void save(
    StringTextSerializer *s, const ::bytesteady::DigramCodec &c);
template void StringTextSerializer::save(
    const ::bytesteady::DigramCodec &c);
template void load(
    StringTextSerializer *s, ::bytesteady::DigramCodec *c);
template void StringTextSerializer::load(
    ::bytesteady::DigramCodec *c);
template void save(
    FileTextSerializer *s, const ::bytesteady::DigramCodec &c);
template void FileTextSerializer::save(
    const ::bytesteady::DigramCodec &c);
template void load(
    FileTextSerializer *s, ::bytesteady::DigramCodec *c);
template void FileTextSerializer::load(
    ::bytesteady::DigramCodec *c);

}  // namespace serializer
}  // namespace thunder
