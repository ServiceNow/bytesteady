/*
 * Copyright 2020 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/bytepair_codec.hpp"

#include <string>
#include <mutex>
#include <thread>
#include <utility>

#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"
#include "thunder/serializer/binary_protocol.hpp"
#include "thunder/serializer/serializer.hpp"
#include "thunder/serializer/static.hpp"
#include "thunder/serializer/text_protocol.hpp"

#include "bytesteady/bytepair_codec-inl.hpp"

#include "thunder/serializer/binary_protocol-inl.hpp"
#include "thunder/serializer/serializer-inl.hpp"
#include "thunder/serializer/static-inl.hpp"
#include "thunder/serializer/text_protocol-inl.hpp"

namespace bytesteady {

bool BytepairCodec::Node::operator<(const Node &b) const {
  return value < b.value ? true : source.size() < b.source.size();
}

BytepairCodec::BytepairCodec(const size_array &g) {
  gram_ = g.size() > 0 ? g[0] : 1;
  thread_size_ = g.size() > 1 ? g[1] : 1;
}

void BytepairCodec::build(const data_callback &callback) {
  // Store the data
  buildDataFromCallback(callback, &data_);
  // Build the gram text table
  buildGramTextFromData(&data_, &gram_text_);
  // Build the replace list
  buildReplaceFromGramText(gram_text_, &replace_);
}

void BytepairCodec::encode(const byte_array &input, byte_array *output) const {
  ::std::string input_string(
       reinterpret_cast< const char * >(&input[0]), input.size());
  for (size_type i = 0; i < replace_.size(); ++i) {
    replaceString(replace_[i].first, replace_[i].second, &input_string);
  }
  output->assign(reinterpret_cast< const uint8_t *>(input_string.c_str()),
                 reinterpret_cast< const uint8_t *>(
                     input_string.c_str() + input_string.size()));
}

void BytepairCodec::decode(const byte_array &input, byte_array *output) const {
  ::std::string input_string(
       reinterpret_cast< const char * >(&input[0]), input.size());
  for (size_type i = 0; i < replace_.size(); ++i) {
    replaceString(replace_[replace_.size() - 1 - i].second,
            replace_[replace_.size() - 1 - i].first,
            &input_string);
  }
  output->assign(reinterpret_cast< const uint8_t *>(input_string.c_str()),
                 reinterpret_cast< const uint8_t *>(
                     input_string.c_str() + input_string.size()));
}

void BytepairCodec::buildDataFromCallback(
    const data_callback &callback, string_array *data) const {
  byte_array input;
  data->clear();
  while (callback(&input) == true) {
    data->push_back(::std::string(
        reinterpret_cast< const char * >(&input[0]), input.size()));
  }
}

void BytepairCodec::buildGramTextFromData(
    string_array *data, string_table *gram_text) const {
  // Generate the original byte sequences corresponding to each gram entry
  string_set base_gram_set;
  gram_text->clear();
  for (const ::std::string &input : *data) {
    for (size_type i = 0; input.size() >= gram_ &&
             i < input.size() - gram_ + 1; i = i + gram_) {
      ::std::string gram = input.substr(i, gram_);
      (*gram_text)[gram] = gram;
      base_gram_set.insert(gram);
    }
  }

  bool found_replace = false;
  do {
    // Build the gram set and digram count
    string_set gram_set;
    size_table digram_count;
    // Always mark base_gram_set, allowing consolidated replace list.
    for (const ::std::string &gram : base_gram_set) {
      gram_set.insert(gram);
    }
    countAll(*data, &gram_set, &digram_count);

    // Find the next available target_key
    byte_array target_byte(gram_, 0);
    ::std::string target(
         reinterpret_cast< const char * >(&target_byte[0]), gram_);
    while (gram_set.find(target) != gram_set.end() &&
           increase(&target_byte) == 0) {
      target.assign(
          reinterpret_cast< const char * >(&target_byte[0]), gram_);
    }
    if (gram_set.find(target) != gram_set.end()) {
      found_replace = false;
      continue;
    }

    // Find the next available replacement
    ::std::string source;
    size_type source_count = 0;
    for (const typename size_table::value_type &pair : digram_count) {
      if (pair.second > source_count) {
        source = pair.first;
        source_count = pair.second;
      }
    }
    if (source_count == 0) {
      found_replace = false;
      continue;
    }

    // Generate the new replace and update the text entry
    found_replace = true;
    replaceAll(source, target, data);
    ::std::string left_source = source.substr(0, gram_);
    ::std::string right_source = source.substr(gram_, gram_);
    (*gram_text)[target] = (*gram_text)[left_source] +
        (*gram_text)[right_source];
  } while (found_replace == true);
}

void BytepairCodec::buildReplaceFromGramText(
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

uint8_t BytepairCodec::increase(byte_array *key) const {
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

void BytepairCodec::replaceString(
    const ::std::string &source, const ::std::string &target,
    ::std::string *output) const {
  size_type position = 0;
  while((position = output->find(source, position)) != ::std::string::npos) {
    // The position must be somewhere on the gram_
    if (position - position / gram_ * gram_ == 0) {
      output->replace(position, source.length(), target);
      position = position + target.length();
    } else {
      ++position;
    }
  }
}

void BytepairCodec::replaceAll(
    const ::std::string &source, const ::std::string &target,
    string_array *data) const {
  size_type index = 0;
  ::std::mutex mutex;
  ::std::vector< ::std::thread > thread;
  for (size_type i = 0; i < thread_size_; ++i) {
    thread.push_back(::std::thread(
        &BytepairCodec::replaceAllJob, this, source, target, data, &index,
        &mutex));
  }
  for (::std::thread &t : thread) {
    t.join();
  }
}

void BytepairCodec::replaceAllJob(
    const ::std::string &source, const ::std::string &target,
    string_array *data, size_type *index, ::std::mutex *mutex) const {
  size_type data_index;
  mutex->lock();
  data_index = (*index)++;
  mutex->unlock();
  while (data_index < data->size()) {
    replaceString(source, target, &(*data)[data_index]);
    mutex->lock();
    data_index = (*index)++;
    mutex->unlock();
  }
}

void BytepairCodec::countAll(
    const string_array &data, string_set *gram_set,
    size_table *digram_count) const {
  size_type index = 0;
  ::std::mutex mutex;
  ::std::vector< ::std::thread > thread;
  for (size_type i = 0; i < thread_size_; ++i) {
    thread.push_back(::std::thread(
        &BytepairCodec::countAllJob, this, data, gram_set, digram_count,
        &index, &mutex));
  }
  for (::std::thread &t : thread) {
    t.join();
  }
}

void BytepairCodec::countAllJob(
    const string_array &data, string_set *gram_set,
    size_table *digram_count, size_type *index, ::std::mutex *mutex) const {
  // Build local gram set and local digram count
  string_set local_gram_set;
  size_table local_digram_count;
  size_type data_index;
  mutex->lock();
  data_index = (*index)++;
  mutex->unlock();
  while (data_index < data.size()) {
    const ::std::string &input = data[data_index];
    for (size_type i = 0; input.size() >= gram_ &&
             i < input.size() - gram_ + 1; i = i + gram_) {
      local_gram_set.insert(input.substr(i, gram_));
    }
    for (size_type i = 0; input.size() >= 2 * gram_ &&
             i < input.size() - 2 * gram_ + 1; i = i + gram_) {
      ::std::string digram = input.substr(i, 2 * gram_);
      if (local_digram_count.find(digram) != local_digram_count.end()) {
        local_digram_count[digram] = local_digram_count[digram] + 1;
      } else {
        local_digram_count[digram] = 1;
      }
    }
    mutex->lock();
    data_index = (*index)++;
    mutex->unlock();
  }

  // Aggregate the results
  mutex->lock();
  for (const ::std::string &gram : local_gram_set) {
    gram_set->insert(gram);
  }
  for (const typename size_table::value_type &pair : local_digram_count) {
    if (digram_count->find(pair.first) != digram_count->end()) {
      (*digram_count)[pair.first] = (*digram_count)[pair.first] + pair.second;
    } else {
      (*digram_count)[pair.first] = pair.second;
    }
  }
  mutex->unlock();
}

typename BytepairCodec::size_type BytepairCodec::gram() const {
  return gram_;
}

void BytepairCodec::set_gram(size_type g) {
  gram_ = g;
}

const typename BytepairCodec::replace_array &BytepairCodec::replace() const {
  return replace_;
}

void BytepairCodec::set_replace(const replace_array &r) {
  replace_ = r;
}

const typename BytepairCodec::string_array &BytepairCodec::data() const {
  return data_;
}

const typename BytepairCodec::string_table &BytepairCodec::gram_text() const {
  return gram_text_;
}

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template void save(
    StringBinarySerializer *s, const ::bytesteady::BytepairCodec &c);
template void StringBinarySerializer::save(
    const ::bytesteady::BytepairCodec &c);
template void load(
    StringBinarySerializer *s, ::bytesteady::BytepairCodec *c);
template void StringBinarySerializer::load(
    ::bytesteady::BytepairCodec *c);
template void save(
    FileBinarySerializer *s, const ::bytesteady::BytepairCodec &c);
template void FileBinarySerializer::save(
    const ::bytesteady::BytepairCodec &c);
template void load(
    FileBinarySerializer *s, ::bytesteady::BytepairCodec *c);
template void FileBinarySerializer::load(
    ::bytesteady::BytepairCodec *c);
template void save(
    StringTextSerializer *s, const ::bytesteady::BytepairCodec &c);
template void StringTextSerializer::save(
    const ::bytesteady::BytepairCodec &c);
template void load(
    StringTextSerializer *s, ::bytesteady::BytepairCodec *c);
template void StringTextSerializer::load(
    ::bytesteady::BytepairCodec *c);
template void save(
    FileTextSerializer *s, const ::bytesteady::BytepairCodec &c);
template void FileTextSerializer::save(
    const ::bytesteady::BytepairCodec &c);
template void load(
    FileTextSerializer *s, ::bytesteady::BytepairCodec *c);
template void FileTextSerializer::load(
    ::bytesteady::BytepairCodec *c);

}  // namespace serializer
}  // namespace thunder
