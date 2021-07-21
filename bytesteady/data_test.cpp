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

#include "bytesteady/data.hpp"

#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <variant>

#include "bytesteady/integer.hpp"
#include "gtest/gtest.h"

namespace bytesteady {
namespace {

template < typename D >
void getSampleThread(D *data, ::std::mutex *print_mutex) {
  typedef typename D::byte_array byte_array;
  typedef typename D::field_array field_array;
  typedef typename D::format_array format_array;
  typedef typename D::index_array index_array;
  typedef typename D::index_pair index_pair;
  typedef typename D::size_type size_type;

  field_array input;
  index_pair label;
  ::std::string thread_id = dynamic_cast< ::std::ostringstream &>(
       ::std::ostringstream() << ::std::hex <<
       ::std::this_thread::get_id()).str();
  while (data->getSample(&input, &label) == true) {
    ::std::lock_guard < ::std::mutex > lock(*print_mutex);
    printf("Data thread = %s", thread_id.c_str());
    for (size_type i = 0; i < input.size(); ++i) {
      if (data->format()[i] == kIndex) {
        const index_array field_index = ::std::get< index_array >(input[i]);
        printf(", input[%lu] = [%lu", i, field_index.size());
        for (size_type j = 0; j < field_index.size(); ++j) {
          printf(", %lu:%g", field_index[j].first, field_index[j].second);
        }
        printf("]");
      } else if (data->format()[i] == kBytes) {
        const byte_array field_bytes = ::std::get< byte_array >(input[i]);
        printf(", input[%lu] = [%lu, ", i, field_bytes.size());
        for (size_type j = 0; j < field_bytes.size(); ++j) {
          printf("%2.2x", field_bytes[j]);
        }
        printf("]");
      }
    }
    printf(", label = %lu:%g", label.first, label.second);
    printf("\n");
  }
}

template < typename D >
void getSampleTest() {
  typedef typename D::format_array format_array;
  typedef typename D::size_type size_type;

  // Create data
  ::std::string file = "bytesteady/unittest_train.txt";
  format_array format = {kBytes, kIndex};
  D data(file, format);

  // Start the thread
  ::std::mutex print_mutex;
  size_type thread_count = 3;
  ::std::vector< ::std::thread > threads;
  for (int iteration = 0; iteration < 3; ++iteration) {
    EXPECT_TRUE(data.rewind());
    for (size_type i = 0; i < thread_count; ++i) {
      threads.push_back(::std::thread(
          getSampleThread< D >, &data, &print_mutex));
    }
    for (size_type i = 0; i < thread_count; ++i) {
      threads[i].join();
    }
    threads.clear();
    EXPECT_EQ(20, data.count());
  }
}

TEST(DataTest, getSampleTest) {
  getSampleTest< DoubleData >();
}

}  // namespace
}  // namespace bytesteady
