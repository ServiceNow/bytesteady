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

#ifndef BYTESTEADY_UNIVERSUM_HPP_
#define BYTESTEADY_UNIVERSUM_HPP_

#include <mutex>
#include <random>
#include <utility>
#include <variant>
#include <vector>

#include "bytesteady/integer.hpp"
#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename T = ::thunder::DoubleTensor >
class Universum {
 public:
  typedef T tensor_type;
  typedef typename T::size_storage size_storage;
  typedef typename T::size_type size_type;
  typedef typename T::value_type value_type;
  typedef ::std::vector< uint8_t > byte_array;
  typedef ::std::pair< size_type, value_type > index_pair;
  typedef ::std::vector< index_pair > index_array;
  typedef ::std::variant< index_array, byte_array > field_variant;
  typedef ::std::vector< field_variant > field_array;

  Universum();

  /*
   * Randomly generate matching fields to data_input. If the field is type
   * kIndex, randomly generate a sequence of indices of the same size, using
   * the corresponding input size. If the field is type kBytes, randomly
   * generate a byte sequence of the same size. The universum_label is generated
   * to be label_size - 1. The weight of input indices and the label is copied
   * to the universum sample.
   */
  bool getSample(
      const size_storage &input_size, size_type label_size,
      const field_array &data_input, const index_pair &data_label,
      field_array *universum_input, index_pair *universum_label);

  size_type count() const;

 private:
  size_type count_;

  ::std::mutex random_mutex_;
  ::std::mt19937_64 generator_;
  ::std::uniform_int_distribution< size_type > index_distribution_;
  ::std::uniform_int_distribution< uint8_t > byte_distribution_;
};

typedef Universum< ::thunder::DoubleTensor > DoubleUniversum;
typedef Universum< ::thunder::FloatTensor > FloatUniversum;

}  // namespace bytesteady

namespace bytesteady{
// Pre-compiled template class instantiation
extern template class Universum< ::thunder::DoubleTensor >;
extern template class Universum< ::thunder::FloatTensor >;
}  // namespace bytesteady

#endif  // BYTESTEADY_UNIVERSUM_HPP_
