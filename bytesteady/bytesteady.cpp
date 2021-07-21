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

#include "bytesteady/bytesteady.hpp"

#include <functional>
#include <unordered_map>

#include "gflags/gflags.h"
#include "glog/logging.h"

template < typename D >
void run() {
  LOG(INFO) << "Joe initialize data, model, train and test for driver";
  D driver;
  if (FLAGS_joe_task == "train") {
    driver.runTrain();
  } else if (FLAGS_joe_task == "test") {
    driver.runTest();
  } else if (FLAGS_joe_task == "infer") {
    driver.runInfer();
  }
}

void checkFlags() {
  if (FLAGS_joe_task != "train" && FLAGS_joe_task != "test" &&
      FLAGS_joe_task != "infer") {
    LOG(FATAL) << "Joe unrecognized command-line flag -joe_task "
               << FLAGS_joe_task;
  }
  if (FLAGS_joe_tensor != "double" && FLAGS_joe_tensor != "float") {
    LOG(FATAL) << "Joe unrecognized command-line flag -joe_tensor "
               << FLAGS_joe_tensor;
  }
  if (FLAGS_joe_hash != "fnv" && FLAGS_joe_hash != "city") {
    LOG(FATAL) << "Joe unrecognized command-line flag -joe_hash "
               << FLAGS_joe_hash;
  }
  if (FLAGS_joe_loss != "nll" && FLAGS_joe_loss != "hinge") {
    LOG(FATAL) << "Joe unrecognized command-line flag -joe_loss "
               << FLAGS_joe_loss;
  }
}

int main(int argc, char *argv[]) {
  using namespace ::bytesteady;

  // Change default arguments
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;
  // Initialize Google gflags to parse command-line arguments
  ::gflags::SetUsageMessage(
       "byteSteady -- a fast classification and tagging tool using byte-level"
       " n-gram embedding");
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  // Initialize Google glogs for logging
  ::google::InitGoogleLogging(argv[0]);
  // Check the parsed flags
  checkFlags();

  // Build the function map and run the task
  ::std::unordered_map< ::std::string, ::std::function< void () > > map;
  map["double-fnv-nll"] = run< DoubleFNVNLLDriver >;
  map["float-fnv-nll"] = run< FloatFNVNLLDriver >;
  map["double-city-nll"] = run< DoubleCityNLLDriver >;
  map["float-city-nll"] = run< FloatCityNLLDriver >;
  map["double-fnv-hinge"] = run< DoubleFNVHingeDriver >;
  map["float-fnv-hinge"] = run< FloatFNVHingeDriver >;
  map["double-city-hinge"] = run< DoubleCityHingeDriver >;
  map["float-city-hinge"] = run< FloatCityHingeDriver >;
  map[FLAGS_joe_tensor + "-" + FLAGS_joe_hash + "-" + FLAGS_joe_loss]();

  // Clean up Google gflags
  ::gflags::ShutDownCommandLineFlags();
  return 0;
}
