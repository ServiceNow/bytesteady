/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/codec_builder.hpp"

#include "bytesteady/data.hpp"
#include "bytesteady/bytehuffman_codec.hpp"
#include "bytesteady/bytepair_codec.hpp"
#include "bytesteady/digram_codec.hpp"
#include "bytesteady/huffman_codec.hpp"
#include "bytesteady/subsample_codec.hpp"

#include "bytesteady/codec_builder-inl.hpp"

namespace bytesteady{
// Pre-compiled template class instantiation
template class CodecBuilder< DoubleData, BytehuffmanCodec >;
template class CodecBuilder< FloatData, BytehuffmanCodec >;
template class CodecBuilder< DoubleData, BytepairCodec >;
template class CodecBuilder< FloatData, BytepairCodec >;
template class CodecBuilder< DoubleData, DigramCodec >;
template class CodecBuilder< FloatData, DigramCodec >;
template class CodecBuilder< DoubleData, HuffmanCodec >;
template class CodecBuilder< FloatData, HuffmanCodec >;
template class CodecBuilder< DoubleData, SubsampleCodec >;
template class CodecBuilder< FloatData, SubsampleCodec >;
}  // namespace bytesteady
