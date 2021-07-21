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

#ifndef BYTESTEADY_CODEC_BUILDER_HPP_
#define BYTESTEADY_CODEC_BUILDER_HPP_

#include <functional>
#include <unordered_map>
#include <vector>

#include "bytesteady/data.hpp"
#include "bytesteady/field_format.hpp"
#include "bytesteady/bytehuffman_codec.hpp"
#include "bytesteady/bytepair_codec.hpp"
#include "bytesteady/digram_codec.hpp"
#include "bytesteady/huffman_codec.hpp"
#include "bytesteady/subsample_codec.hpp"

namespace bytesteady {

template < typename D = Data<>, typename C = HuffmanCodec >
class CodecBuilder {
 public:
  typedef D data_type;
  typedef C codec_type;
  typedef typename D::field_array field_array;
  typedef typename D::index_pair index_pair;
  typedef typename C::byte_array byte_array;
  typedef typename C::data_callback data_callback;
  typedef typename C::size_type size_type;
  typedef typename C::size_array size_array;
  typedef ::std::vector< size_array > gram_array;
  typedef ::std::unordered_map< size_type, C > codec_table;

  struct Local {
    field_array data_input;
    index_pair data_label;
    size_type data_field;
  };
  typedef Local local_type;
  typedef ::std::function< void (const Local &) > callback_type;

  CodecBuilder(D *d, codec_table *c, const gram_array &g);

  /*
   * Returns false if data cannot rewind
   */
  bool build(const callback_type &callback);

  const gram_array &gram();

 private:
  D *data_;
  codec_table *codec_;
  gram_array gram_;
};

typedef CodecBuilder< DoubleData, BytehuffmanCodec >
DoubleBytehuffmanCodecBuilder;
typedef CodecBuilder< FloatData, BytehuffmanCodec >
FloatBytehuffmanCodecBuilder;
typedef CodecBuilder< DoubleData, BytepairCodec > DoubleBytepairCodecBuilder;
typedef CodecBuilder< FloatData, BytepairCodec > FloatBytepairCodecBuilder;
typedef CodecBuilder< DoubleData, DigramCodec > DoubleDigramCodecBuilder;
typedef CodecBuilder< FloatData, DigramCodec > FloatDigramCodecBuilder;
typedef CodecBuilder< DoubleData, HuffmanCodec > DoubleHuffmanCodecBuilder;
typedef CodecBuilder< FloatData, HuffmanCodec > FloatHuffmanCodecBuilder;
typedef CodecBuilder< DoubleData, SubsampleCodec > DoubleSubsampleCodecBuilder;
typedef CodecBuilder< FloatData, SubsampleCodec > FloatSubsampleCodecBuilder;

}  // namespace bytesteady

namespace bytesteady{
// Pre-compiled template class instantiation
extern template class CodecBuilder< DoubleData, BytehuffmanCodec >;
extern template class CodecBuilder< FloatData, BytehuffmanCodec >;
extern template class CodecBuilder< DoubleData, BytepairCodec >;
extern template class CodecBuilder< FloatData, BytepairCodec >;
extern template class CodecBuilder< DoubleData, DigramCodec >;
extern template class CodecBuilder< FloatData, DigramCodec >;
extern template class CodecBuilder< DoubleData, HuffmanCodec >;
extern template class CodecBuilder< FloatData, HuffmanCodec >;
extern template class CodecBuilder< DoubleData, SubsampleCodec >;
extern template class CodecBuilder< FloatData, SubsampleCodec >;
}  // namespace bytesteady

#endif  // BYTESTEADY_CODEC_BUILDER_HPP_
