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

#ifndef BYTESTEADY_BIT_ARRAY_HPP_
#define BYTESTEADY_BIT_ARRAY_HPP_

#include <list>
#include <vector>

#include "bytesteady/integer.hpp"

namespace bytesteady {

class BitArray {
 public:
  typedef ::std::list< uint8_t > byte_list;
  typedef ::std::vector< uint8_t > byte_array;
  typedef typename byte_list::value_type value_type;
  typedef typename byte_list::size_type size_type;
  typedef typename byte_list::reference reference;
  typedef typename byte_list::const_reference const_reference;

  BitArray();
  BitArray(const byte_list &data);
  BitArray(const byte_list &prefix, const byte_list &data,
           const byte_list &postfix);
  BitArray(const byte_array &data);

  void pushFront(value_type bit);
  value_type popFront();
  value_type front();

  void pushBack(value_type bit);
  value_type popBack();
  value_type back();

  void getPaddedBytes(value_type pad_bit, byte_array *output);
  byte_array getPaddedBytes(value_type pad_bit);

  const byte_list &prefix() const;
  void set_prefix(const byte_list &prefix);
  const byte_list &data() const;
  void set_data(const byte_list &data);
  const byte_list &postfix() const;
  void set_postfix(const byte_list &postfix);

  size_type size();
  void clear();

 private:
  byte_list prefix_;
  byte_list data_;
  byte_list postfix_;
};

}  // namespace bytesteady

#endif  // BYTESTEADY_BIT_ARRAY_HPP_
