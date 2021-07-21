/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_DATA_HPP_
#define BYTESTEADY_DATA_HPP_

#include <cstdio>
#include <mutex>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "bytesteady/field_format.hpp"
#include "bytesteady/integer.hpp"
#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename T = ::thunder::DoubleTensor >
class Data {
 public:
  typedef T tensor_type;
  typedef typename T::size_type size_type;
  typedef typename T::value_type value_type;
  typedef ::std::vector< uint8_t > byte_array;
  typedef ::std::pair< size_type, value_type > index_pair;
  typedef ::std::vector< index_pair > index_array;
  typedef ::std::variant< index_array, byte_array > field_variant;
  typedef ::std::vector< field_variant > field_array;
  typedef ::std::vector< FieldFormat > format_array;

  // File name and format
  Data(const ::std::string &fn, const format_array &ft = {kIndex});
  // Close the file
  ~Data();

  // Rewind the file position and reset counter
  bool rewind();
  // Set the position of file and counter
  bool seek(long os, size_type ct);

  // Get a sample. Returns false if there is a read error
  bool getSample(field_array *input, index_pair *label = nullptr,
                 size_type *count = nullptr);

  void lock();
  void unlock();

  const ::std::string &file() const;
  void set_file(const ::std::string &fn);

  const format_array &format() const;
  void set_format(const format_array &ft);

  size_type count() const;
  long offset() const;

 private:
  ::std::string file_;
  format_array format_;

  ::std::mutex file_mutex_;
  size_type count_;
  FILE *fp_;

  // Scratch space for reading data
  byte_array bytes_;
  index_array indices_;
};

typedef Data< ::thunder::DoubleTensor > DoubleData;
typedef Data< ::thunder::FloatTensor > FloatData;
// Already exists: typedef DoubleData Data;

}  // namespace bytesteady

namespace bytesteady {
// Pre-compiled template class instantiation
extern template class Data< ::thunder::DoubleTensor >;
extern template class Data< ::thunder::FloatTensor >;
}  // namespace bytesteady

#endif  // BYTESTEADY_DATA_HPP_
