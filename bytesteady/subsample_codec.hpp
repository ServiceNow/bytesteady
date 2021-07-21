/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_SUBSAMPLE_CODEC_HPP_
#define BYTESTEADY_SUBSAMPLE_CODEC_HPP_

#include <functional>
#include <vector>

#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"

namespace bytesteady {

class SubsampleCodec {
 public:
  typedef typename ::std::vector< uint8_t > byte_array;
  typedef typename byte_array::size_type size_type;
  typedef ::std::vector< size_type > size_array;
  typedef ::std::function< bool (byte_array *input) > data_callback;

  // Construct the codec
  SubsampleCodec(const size_array &f = {1});

  // Build the codec from a callback
  void build(const data_callback &callback);
  // Encode the data
  void encode(const byte_array &input, byte_array *output) const;
  // Decode the data
  void decode(const byte_array &input, byte_array *output) const;

  size_type factor() const;
  void set_factor(size_type f);

 private:
  size_type factor_;
};

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template < typename S >
void save(S *s, const ::bytesteady::SubsampleCodec &c);

template < typename S >
void load(S *s, ::bytesteady::SubsampleCodec *c);

}  // namespace serializer
}  // namespace thunder

namespace thunder {
namespace serializer {

// Pre-compiled template serializer instantiation
extern template void save(
    StringBinarySerializer *s, const ::bytesteady::SubsampleCodec &c);
extern template void StringBinarySerializer::save(
    const ::bytesteady::SubsampleCodec &c);
extern template void load(
    StringBinarySerializer *s, ::bytesteady::SubsampleCodec *c);
extern template void StringBinarySerializer::load(
    ::bytesteady::SubsampleCodec *c);
extern template void save(
    FileBinarySerializer *s, const ::bytesteady::SubsampleCodec &c);
extern template void FileBinarySerializer::save(
    const ::bytesteady::SubsampleCodec &c);
extern template void load(
    FileBinarySerializer *s, ::bytesteady::SubsampleCodec *c);
extern template void FileBinarySerializer::load(
    ::bytesteady::SubsampleCodec *c);
extern template void save(
    StringTextSerializer *s, const ::bytesteady::SubsampleCodec &c);
extern template void StringTextSerializer::save(
    const ::bytesteady::SubsampleCodec &c);
extern template void load(
    StringTextSerializer *s, ::bytesteady::SubsampleCodec *c);
extern template void StringTextSerializer::load(
    ::bytesteady::SubsampleCodec *c);
extern template void save(
    FileTextSerializer *s, const ::bytesteady::SubsampleCodec &c);
extern template void FileTextSerializer::save(
    const ::bytesteady::SubsampleCodec &c);
extern template void load(
    FileTextSerializer *s, ::bytesteady::SubsampleCodec *c);
extern template void FileTextSerializer::load(
    ::bytesteady::SubsampleCodec *c);

}  // namespace thunder
}  // namespace serializer

#endif  // BYTESTEADY_SUBSAMPLE_CODEC_HPP_
