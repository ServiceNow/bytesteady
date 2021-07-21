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
