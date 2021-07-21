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
