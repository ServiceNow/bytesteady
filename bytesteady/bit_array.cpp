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

#include "bytesteady/bit_array.hpp"

#include <list>
#include <vector>

#include "bytesteady/integer.hpp"

namespace bytesteady {

BitArray::BitArray() {}

BitArray::BitArray(const byte_list &data) : data_(data) {}

BitArray::BitArray(
    const byte_list &prefix, const byte_list &data, const byte_list &postfix) :
    prefix_(prefix), data_(data), postfix_(postfix) {}

BitArray::BitArray(const byte_array &data) {
  for (const uint8_t &byte : data) {
    data_.push_back(byte);
  }
}

void BitArray::pushFront(value_type bit) {
  prefix_.push_front(bit);
  if (prefix_.size() > 8) {
    value_type byte = 0;
    value_type base = 0b00000001;
    for (size_type i = 0; i < 8; ++i) {
      byte = byte + prefix_.back() * base;
      prefix_.pop_back();
      base = base << 1;
    }
    data_.push_front(byte);
  }
}

typename BitArray::value_type BitArray::popFront() {
  value_type bit = front();
  prefix_.pop_front();
  return bit;
}

typename BitArray::value_type BitArray::front() {
  if (prefix_.size() == 0) {
    if (data_.size() == 0) {
      prefix_.push_back(postfix_.front());
      postfix_.pop_front();
    } else {
      value_type byte = data_.front();
      data_.pop_front();
      for (size_type i = 0; i < 8; ++i) {
        prefix_.push_front(byte & 0b00000001);
        byte = byte >> 1;
      }
    }
  }
  return prefix_.front();
}

void BitArray::pushBack(value_type bit) {
  postfix_.push_back(bit);
  if (postfix_.size() > 8) {
    value_type byte = 0;
    value_type base = 0b10000000;
    for (size_type i = 0; i < 8; ++i) {
      byte = byte + postfix_.front() * base;
      postfix_.pop_front();
      base = base >> 1;
    }
    data_.push_back(byte);
  }
}

typename BitArray::value_type BitArray::popBack() {
  value_type bit = back();
  postfix_.pop_back();
  return bit;
}

typename BitArray::value_type BitArray::back() {
  if (postfix_.size() == 0) {
    if (data_.size() == 0) {
      postfix_.push_front(prefix_.back());
      prefix_.pop_back();
    } else {
      value_type byte = data_.back();
      data_.pop_back();
      for (size_type i = 0; i < 8; ++i) {
        postfix_.push_front(byte & 0b00000001);
        byte = byte >> 1;
      }
    }
  }
  return postfix_.back();
}

void BitArray::getPaddedBytes(value_type pad_bit, byte_array *output) {
  output->clear();

  // Consolidate the prefix
  if (prefix_.size() == 8) {
    value_type byte = 0;
    value_type base = 0b00000001;
    for (size_type i = 0; i < 8; ++i) {
      byte = byte + prefix_.back() * base;
      prefix_.pop_back();
      base = base << 1;
    }
    data_.push_front(byte);
  }

  value_type byte = 0;
  value_type base = 0b00000001;
  if (prefix_.size() == 0) {
    // Put all data in the output array
    for (const value_type &byte : data_) {
      output->push_back(byte);
    }
  } else {
    // Read from prefix
    for (const value_type &bit : prefix_) {
      byte = byte << 1;
      byte = byte + bit;
      base = base << 1;
      if (base == 0) {
        output->push_back(byte);
        byte = 0;
        base = 0b00000001;
      }
    }
    // Read from data
    for (value_type data_byte : data_) {
      value_type data_base = 0b10000000;
      for (size_type i = 0; i < 8; ++i) {
        value_type bit = (data_byte & data_base) > 0 ? 1 : 0;
        data_base = data_base >> 1;
        byte = byte << 1;
        byte = byte + bit;
        base = base << 1;
        if (base == 0) {
          output->push_back(byte);
          byte = 0;
          base = 0b00000001;
        }
      }
    }
  }

  // Read from postfix
  for (const value_type &bit : postfix_) {
    byte = byte << 1;
    byte = byte + bit;
    base = base << 1;
    if (base == 0) {
      output->push_back(byte);
      byte = 0;
      base = 0b00000001;
    }
  }

  // Pad the last byte
  while (base != 0b00000001) {
    byte = byte << 1;
    byte = byte + pad_bit;
    base = base << 1;
    if (base == 0) {
      output->push_back(byte);
      byte = 0;
      base = 0b00000001;
    }
  }
}

typename BitArray::byte_array BitArray::getPaddedBytes(value_type pad_bit) {
  byte_array output;
  getPaddedBytes(pad_bit, &output);
  return output;
}

const typename BitArray::byte_list &BitArray::prefix() const {
  return prefix_;
}

void BitArray::set_prefix(const byte_list &prefix) {
  prefix_ = prefix;
}

const typename BitArray::byte_list &BitArray::data() const {
  return data_;
}

void BitArray::set_data(const byte_list &data) {
  data_ = data;
}

const typename BitArray::byte_list &BitArray::postfix() const {
  return postfix_;
}

void BitArray::set_postfix(const byte_list &postfix) {
  postfix_ = postfix;
}

typename BitArray::size_type BitArray::size() {
  return prefix_.size() + 8 * data_.size() + postfix_.size();
}

void BitArray::clear() {
  prefix_.clear();
  data_.clear();
  postfix_.clear();
}

}  // namespace bytesteady
