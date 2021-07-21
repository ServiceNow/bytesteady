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
