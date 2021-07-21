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

#include "bytesteady/universum.hpp"

#include <limits>
#include <mutex>
#include <random>
#include <utility>

namespace bytesteady {

template < typename T >
Universum< T >::Universum() :
    generator_(::std::random_device()()),
    index_distribution_(0, ::std::numeric_limits< size_type >::max()),
    byte_distribution_(0, 255) {}

template < typename T >
bool Universum< T >::getSample(
    const size_storage &input_size, size_type label_size,
    const field_array &data_input, const index_pair &data_label,
    field_array *universum_input, index_pair *universum_label) {
  universum_input->clear();
  ::std::lock_guard< ::std::mutex > lock(random_mutex_);
  const index_array *data_field_index;
  const byte_array *data_field_bytes;
  for (size_type i = 0; i < data_input.size(); ++i) {
    if ((data_field_index = ::std::get_if< index_array >(&data_input[i]))
        != nullptr) {
      index_array universum_field_index;
      for (size_type j = 0; j < data_field_index->size(); ++j) {
        universum_field_index.push_back(::std::make_pair(
            index_distribution_(generator_) % input_size[i],
            (*data_field_index)[j].second));
      }
      universum_input->push_back(universum_field_index);
    } else if ((data_field_bytes = ::std::get_if< byte_array >(&data_input[i]))
               != nullptr) {
      byte_array universum_field_bytes(data_field_bytes->size());
      for (size_type j = 0; j < universum_field_bytes.size(); ++j) {
        universum_field_bytes[j] = byte_distribution_(generator_);
      }
      universum_input->push_back(universum_field_bytes);
    } else {
      return false;
    }
  }
  universum_label->first = label_size - 1;
  universum_label->second = data_label.second;
  ++count_;
  return true;
}

template < typename T >
typename Universum< T >::size_type Universum< T >::count() const {
  return count_;
}

}  // namespace bytesteady
