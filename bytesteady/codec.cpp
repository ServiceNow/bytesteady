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

#include "bytesteady/codec.hpp"

#include "gflags/gflags.h"
#include "glog/logging.h"

template < typename D >
void run() {
  LOG(INFO) << "Joe initialize data for driver";
  D driver;
  if (FLAGS_joe_task == "build") {
    driver.runBuild();
  } else if (FLAGS_joe_task == "encode") {
    driver.runEncode();
  } else if (FLAGS_joe_task == "decode") {
    driver.runDecode();
  }
}

void checkFlags() {
  if (FLAGS_joe_task != "build" && FLAGS_joe_task != "encode" &&
      FLAGS_joe_task != "decode") {
    LOG(FATAL) << "Joe unrecognized command-line flags -joe_task "
               << FLAGS_joe_task;
  }
  if (FLAGS_joe_codec != "bytehuffman" && FLAGS_joe_codec != "bytepair" &&
      FLAGS_joe_codec != "digram" && FLAGS_joe_codec != "huffman" &&
      FLAGS_joe_codec != "subsample") {
    LOG(FATAL) << "Joe unrecognized command-line flags -joe_codec "
               << FLAGS_joe_codec;    
  }
}

int main(int argc, char *argv[]) {
  using namespace ::bytesteady;

  // Change default arguments
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;
  // Initialize Google gflags to parse command-line arguments
  ::gflags::SetUsageMessage(
       "byteSteady/codec -- building, encoding and decoding for byteSteady"
       " datasets");
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  // Initialize Google glogs for logging
  ::google::InitGoogleLogging(argv[0]);
  // Check the parsed flags
  checkFlags();

  if (FLAGS_joe_codec == "bytehuffman") {
    run< DoubleBytehuffmanCodecDriver >();
  } else if (FLAGS_joe_codec == "bytepair") {
    run< DoubleBytepairCodecDriver >();
  } else if (FLAGS_joe_codec == "digram") {
    run< DoubleDigramCodecDriver >();
  } else if (FLAGS_joe_codec == "huffman") {
    run< DoubleHuffmanCodecDriver >();
  } else if (FLAGS_joe_codec == "subsample") {
    run< DoubleSubsampleCodecDriver >();
  }

  // Clean up Google gflags
  ::gflags::ShutDownCommandLineFlags();
  return 0;
}
