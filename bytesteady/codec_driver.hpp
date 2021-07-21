/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#ifndef BYTESTEADY_CODEC_DRIVER_HPP_
#define BYTESTEADY_CODEC_DRIVER_HPP_

#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "bytesteady/data.hpp"
#include "bytesteady/codec_builder.hpp"
#include "bytesteady/codec_coder.hpp"

namespace bytesteady {

template < typename D = Data<>, typename B = CodecBuilder<>,
           typename C = CodecCoder<> >
class CodecDriver {
 public:
  typedef D data_type;
  typedef B builder_type;
  typedef C coder_type;
  typedef typename D::byte_array byte_array;
  typedef typename D::field_array field_array;
  typedef typename D::field_variant field_variant;
  typedef typename D::format_array format_array;
  typedef typename D::index_array index_array;
  typedef typename D::index_pair index_pair;
  typedef typename B::codec_table codec_table;
  typedef typename B::gram_array gram_array;
  typedef typename B::local_type builder_local;
  typedef typename B::callback_type builder_callback;
  typedef typename B::size_array size_array;
  typedef typename B::size_type size_type;
  typedef typename C::local_type coder_local;
  typedef typename C::callback_type coder_callback;
  typedef typename ::std::chrono::duration< double > duration_type;
  typedef typename ::std::chrono::steady_clock::time_point time_point;

  CodecDriver();

  void runBuild();
  void runEncode();
  void runDecode();

  void save();
  void load();

  void buildCallback(const builder_local &local);
  void encodeCallback(const coder_local &local);
  void decodeCallback(const coder_local &local);

  void buildLog(const builder_local &local);
  void codeLog(const coder_local &local, const ::std::string &task);

  bool checkLog();

  static format_array parseDataFormat();
  static gram_array parseBuilderGram();
  static ::std::string fieldToString(const field_variant &f);

  const D &data();
  const codec_table &codec();

 private:
  D data_;
  codec_table codec_;

  duration_type log_interval_;
  time_point log_time_point_;
  ::std::mutex log_time_mutex_;
};

typedef CodecDriver<
  DoubleData, DoubleBytehuffmanCodecBuilder, DoubleBytehuffmanCodecCoder >
DoubleBytehuffmanCodecDriver;
typedef CodecDriver<
  FloatData, FloatBytehuffmanCodecBuilder, FloatBytehuffmanCodecCoder >
FloatBytehuffmanCodecDriver;
typedef CodecDriver<
  DoubleData, DoubleBytepairCodecBuilder, DoubleBytepairCodecCoder >
DoubleBytepairCodecDriver;
typedef CodecDriver<
  FloatData, FloatBytepairCodecBuilder, FloatBytepairCodecCoder >
FloatBytepairCodecDriver;
typedef CodecDriver<
  DoubleData, DoubleDigramCodecBuilder, DoubleDigramCodecCoder >
DoubleDigramCodecDriver;
typedef CodecDriver<
  FloatData, FloatDigramCodecBuilder, FloatDigramCodecCoder >
FloatDigramCodecDriver;
typedef CodecDriver<
  DoubleData, DoubleHuffmanCodecBuilder, DoubleHuffmanCodecCoder >
DoubleHuffmanCodecDriver;
typedef CodecDriver<
  FloatData, FloatHuffmanCodecBuilder, FloatHuffmanCodecCoder >
FloatHuffmanCodecDriver;
typedef CodecDriver<
  DoubleData, DoubleSubsampleCodecBuilder, DoubleSubsampleCodecCoder >
DoubleSubsampleCodecDriver;
typedef CodecDriver<
  FloatData, FloatSubsampleCodecBuilder, FloatSubsampleCodecCoder >
FloatSubsampleCodecDriver;
}  // namespace bytesteady

namespace bytesteady {
extern template class CodecDriver<
  DoubleData, DoubleBytehuffmanCodecBuilder, DoubleBytehuffmanCodecCoder >;
extern template class CodecDriver<
  FloatData, FloatBytehuffmanCodecBuilder, FloatBytehuffmanCodecCoder >;
extern template class CodecDriver<
  DoubleData, DoubleBytepairCodecBuilder, DoubleBytepairCodecCoder >;
extern template class CodecDriver<
  FloatData, FloatBytepairCodecBuilder, FloatBytepairCodecCoder >;
extern template class CodecDriver<
  DoubleData, DoubleDigramCodecBuilder, DoubleDigramCodecCoder >;
extern template class CodecDriver<
  FloatData, FloatDigramCodecBuilder, FloatDigramCodecCoder >;
extern template class CodecDriver<
  DoubleData, DoubleHuffmanCodecBuilder, DoubleHuffmanCodecCoder >;
extern template class CodecDriver<
  FloatData, FloatHuffmanCodecBuilder, FloatHuffmanCodecCoder >;
extern template class CodecDriver<
  DoubleData, DoubleSubsampleCodecBuilder, DoubleSubsampleCodecCoder >;
extern template class CodecDriver<
  FloatData, FloatSubsampleCodecBuilder, FloatSubsampleCodecCoder >;
}  // namespace bytesteady

#endif  // BYTESTEADY_CODEC_DRIVER_HPP_
