/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/codec_coder.hpp"

#include "bytesteady/data.hpp"
#include "bytesteady/bytehuffman_codec.hpp"
#include "bytesteady/bytepair_codec.hpp"
#include "bytesteady/digram_codec.hpp"
#include "bytesteady/huffman_codec.hpp"
#include "bytesteady/subsample_codec.hpp"

#include "bytesteady/codec_coder-inl.hpp"

namespace bytesteady {
// Pre-compiled template class instantiation
template class CodecCoder< DoubleData, BytehuffmanCodec >;
template class CodecCoder< FloatData, BytehuffmanCodec >;
template class CodecCoder< DoubleData, BytepairCodec >;
template class CodecCoder< FloatData, BytepairCodec >;
template class CodecCoder< DoubleData, DigramCodec >;
template class CodecCoder< FloatData, DigramCodec >;
template class CodecCoder< DoubleData, HuffmanCodec >;
template class CodecCoder< FloatData, HuffmanCodec >;
template class CodecCoder< DoubleData, SubsampleCodec >;
template class CodecCoder< FloatData, SubsampleCodec >;
}  // namespace bytesteady
