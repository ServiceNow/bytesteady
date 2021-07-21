/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/subsample_codec.hpp"

#include <functional>

#include "bytesteady/integer.hpp"

#include "thunder/serializer.hpp"
#include "thunder/serializer/binary_protocol.hpp"
#include "thunder/serializer/serializer.hpp"
#include "thunder/serializer/static.hpp"
#include "thunder/serializer/text_protocol.hpp"

#include "bytesteady/subsample_codec-inl.hpp"

#include "thunder/serializer/binary_protocol-inl.hpp"
#include "thunder/serializer/serializer-inl.hpp"
#include "thunder/serializer/static-inl.hpp"
#include "thunder/serializer/text_protocol-inl.hpp"

namespace bytesteady {

SubsampleCodec::SubsampleCodec(const size_array &g) :
    factor_(g.size() > 0 ? g[0] : 1) {};

void SubsampleCodec::build(const data_callback &callback) {
  // Do not need to do anything
  return;
}

void SubsampleCodec::encode(
    const byte_array &input, byte_array *output) const {
  output->clear();
  for (const uint8_t &byte : input) {
    output->push_back(byte / factor_);
  }
}

void SubsampleCodec::decode(
    const byte_array &input, byte_array *output) const {
  output->clear();
  for (const uint8_t &byte : input) {
    output->push_back(byte * factor_);
  }
}

SubsampleCodec::size_type SubsampleCodec::factor() const {
  return factor_;
}

void SubsampleCodec::set_factor(size_type f) {
  factor_ = f;
}

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template void save(
    StringBinarySerializer *s, const ::bytesteady::SubsampleCodec &c);
template void StringBinarySerializer::save(
    const ::bytesteady::SubsampleCodec &c);
template void load(
    StringBinarySerializer *s, ::bytesteady::SubsampleCodec *c);
template void StringBinarySerializer::load(
    ::bytesteady::SubsampleCodec *c);
template void save(
    FileBinarySerializer *s, const ::bytesteady::SubsampleCodec &c);
template void FileBinarySerializer::save(
    const ::bytesteady::SubsampleCodec &c);
template void load(
    FileBinarySerializer *s, ::bytesteady::SubsampleCodec *c);
template void FileBinarySerializer::load(
    ::bytesteady::SubsampleCodec *c);
template void save(
    StringTextSerializer *s, const ::bytesteady::SubsampleCodec &c);
template void StringTextSerializer::save(
    const ::bytesteady::SubsampleCodec &c);
template void load(
    StringTextSerializer *s, ::bytesteady::SubsampleCodec *c);
template void StringTextSerializer::load(
    ::bytesteady::SubsampleCodec *c);
template void save(
    FileTextSerializer *s, const ::bytesteady::SubsampleCodec &c);
template void FileTextSerializer::save(
    const ::bytesteady::SubsampleCodec &c);
template void load(
    FileTextSerializer *s, ::bytesteady::SubsampleCodec *c);
template void FileTextSerializer::load(
    ::bytesteady::SubsampleCodec *c);

}  // namespace serializer
}  // namespace thunder
