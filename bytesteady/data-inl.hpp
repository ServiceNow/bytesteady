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

#include "bytesteady/data.hpp"

#include <cstdio>
#include <cstring>
#include <mutex>
#include <utility>
#include <variant>
#include <vector>

#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename T >
Data< T >::Data(
    const ::std::string &fn, const format_array &ft) :
    file_(fn), format_(ft), count_(0), fp_(::std::fopen(fn.c_str(), "r")) {}

template < typename T >
Data< T >::~Data() {
  ::std::lock_guard< ::std::mutex > lock(file_mutex_);
  ::std::fclose(fp_);
}

template < typename T >
bool Data< T >::rewind() {
  ::std::lock_guard< ::std::mutex > lock(file_mutex_);
  if (fp_ == nullptr) {
    fp_ = ::std::fopen(file_.c_str(), "r");
  }
  if (fp_ == nullptr) {
    return false;
  }
  ::std::rewind(fp_);
  count_ = 0;
  return true;
}

template < typename T >
bool Data< T >::seek(long os, size_type ct) {
  ::std::lock_guard< ::std::mutex > lock(file_mutex_);
  if (fp_ == nullptr) {
    fp_ = ::std::fopen(file_.c_str(), "r");
  }
  if (fp_ == nullptr) {
    return false;
  }
  if (::std::fseek(fp_, os, SEEK_SET) != 0) {
    return false;
  }
  count_ = ct;
  return true;
}

template < typename T >
bool Data< T >::getSample(
    field_array *input, index_pair *label, size_type *count) {
  ::std::lock_guard< ::std::mutex > lock(file_mutex_);
  input->clear();
  size_type index;
  double weight;
  char separator[2];
  char hex[3];
  for (size_type i = 0; i < format_.size(); ++i) {
    if (format_[i] == kIndex) {
      indices_.clear();
      // Read the first index
      if (fscanf(fp_, "%lu", &index) != 1) {
        return false;
      }
      weight = 1.0;
      if (fscanf(fp_, " %1[:]", separator) == 1 && separator[0] == ':') {
        // Saw a ':', the next value is weight
        if (fscanf(fp_, "%lg", &weight) != 1) {
          return false;
        }
      }
      indices_.push_back(::std::make_pair(
          index, static_cast< value_type >(weight)));
      while (fscanf(fp_, " %1[,]", separator) == 1 && separator[0] == ',') {
        // Saw a ',', read the next index
        if (fscanf(fp_, "%lu", &index) != 1) {
          return false;
        }
        weight = 1.0;
        if (fscanf(fp_, " %1[:]", separator) == 1 && separator[0] == ':') {
          // Saw a ':', the next value is weight
          if (fscanf(fp_, "%lg", &weight) != 1) {
            return false;
          }
        }
        indices_.push_back(::std::make_pair(
            index, static_cast< value_type >(weight)));
      }
      input->push_back(indices_);
    } else if (format_[i] == kBytes) {
      bytes_.clear();
      // Skip the beginning white spaces and read the first hex
      if (fscanf(fp_, " %2[0123456789ABCDEFabcdef]", hex) != 1 ||
          hex[0] == '\0' || hex[1] == '\0') {
        return false;
      }
      bytes_.push_back(
          static_cast< uint8_t >(::std::strtoul(hex, nullptr, 16)));
      // No skipping of beginning white spaces for the second hex and after
      while (fscanf(fp_, "%2[0123456789ABCDEFabcdef]", hex) == 1 &&
             hex[0] != '\0' && hex[1] != '\0') {
        bytes_.push_back(
            static_cast< uint8_t >(::std::strtoul(hex, nullptr, 16)));
      }
      input->push_back(bytes_);
    }
  }

  // Read label if the pointer is not nullptr
  if (label != nullptr) {
    if (fscanf(fp_, "%lu", &index) != 1) {
      return false;
    }
    weight = 1.0;
    if (fscanf(fp_, " %1[:]", separator) == 1 && separator[0] == ':') {
      // Saw a ':', the next value is weight
      if (fscanf(fp_, "%lg", &weight) != 1) {
        return false;
      }
    }
    label->first = index;
    label->second = static_cast< value_type >(weight);
  }

  if (count != nullptr) {
    *count = count_;
  }
  ++count_;
  return true;
}

template < typename T >
void Data< T >::lock() {
  file_mutex_.lock();
}

template < typename T >
void Data< T >::unlock() {
  file_mutex_.unlock();
}

template < typename T >
const ::std::string &Data< T >::file() const {
  return file_;
}

template < typename T >
void Data< T >::set_file(const ::std::string &fn) {
  file_ = fn;
}

template < typename T >
const typename Data< T >::format_array &Data< T >::format() const {
  return format_;
}

template < typename T >
void Data< T >::set_format(const format_array &ft) {
  format_ = ft;
}

template < typename T >
typename Data< T >::size_type Data< T >::count() const {
  return count_;
}

template < typename T >
long Data< T >::offset() const {
  return ::std::ftell(fp_);
}

}  // namespace bytesteady
