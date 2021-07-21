/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
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
