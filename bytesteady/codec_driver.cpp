/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/codec_driver.hpp"

#include "bytesteady/codec_builder.hpp"
#include "bytesteady/codec_coder.hpp"
#include "bytesteady/data.hpp"

#include "bytesteady/codec_driver-inl.hpp"

namespace bytesteady {
template class CodecDriver<
  DoubleData, DoubleBytehuffmanCodecBuilder, DoubleBytehuffmanCodecCoder >;
template class CodecDriver<
  FloatData, FloatBytehuffmanCodecBuilder, FloatBytehuffmanCodecCoder >;
template class CodecDriver<
  DoubleData, DoubleBytepairCodecBuilder, DoubleBytepairCodecCoder >;
template class CodecDriver<
  FloatData, FloatBytepairCodecBuilder, FloatBytepairCodecCoder >;
template class CodecDriver<
  DoubleData, DoubleDigramCodecBuilder, DoubleDigramCodecCoder >;
template class CodecDriver<
  FloatData, FloatDigramCodecBuilder, FloatDigramCodecCoder >;
template class CodecDriver<
  DoubleData, DoubleHuffmanCodecBuilder, DoubleHuffmanCodecCoder >;
template class CodecDriver<
  FloatData, FloatHuffmanCodecBuilder, FloatHuffmanCodecCoder >;
template class CodecDriver<
  DoubleData, DoubleSubsampleCodecBuilder, DoubleSubsampleCodecCoder >;
template class CodecDriver<
  FloatData, FloatSubsampleCodecBuilder, FloatSubsampleCodecCoder >;
}  // namespace bytesteady
