/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/codec_driver.hpp"

#include "bytesteady/codec_flags.hpp"
#include "gtest/gtest.h"

namespace bytesteady {
namespace {

template < typename D >
void buildEncodeDecodeTest() {
  FLAGS_data_file = "bytesteady/unittest_train.txt";
  FLAGS_data_format = "kBytes,kIndex";
  FLAGS_builder_gram = "{1,2,4},{}";
  FLAGS_coder_output = "/tmp/output.txt";
  FLAGS_coder_thread_size = 3;
  FLAGS_driver_file = "/tmp/codec.tdb";
  FLAGS_driver_debug = true;
  FLAGS_driver_log_interval = 0.0;
  FLAGS_driver_log_precision = 16;

  D build_driver;
  build_driver.runBuild();

  D encode_driver;
  encode_driver.runEncode();

  D decode_driver;
  decode_driver.runDecode();
}

TEST(CodecDriverTest, buildEncodeDecodeTest) {
  buildEncodeDecodeTest< DoubleHuffmanCodecDriver >();
}

}  // namespace
}  // namespace bytesteady
