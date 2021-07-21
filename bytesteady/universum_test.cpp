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

#include "bytesteady/universum.hpp"

#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <variant>

#include "bytesteady/data.hpp"
#include "bytesteady/field_format.hpp"
#include "gtest/gtest.h"

namespace bytesteady {
namespace {

template < typename D, typename U >
void getSampleThread(
    const typename U::size_storage &input_size,
    typename U::size_type label_size, typename U::size_type universum_size,
    D *data, U *universum, ::std::mutex *print_mutex) {
  typedef typename U::byte_array byte_array;
  typedef typename U::field_array field_array;
  typedef typename U::index_array index_array;
  typedef typename U::index_pair index_pair;
  typedef typename U::size_storage size_storage;
  typedef typename U::size_type size_type;
  typedef typename U::value_type value_type;

  field_array data_input;
  index_pair data_label;
  field_array universum_input;
  index_pair universum_label;
  ::std::string thread_id = dynamic_cast< ::std::ostringstream &>(
       ::std::ostringstream() << ::std::hex <<
       ::std::this_thread::get_id()).str();
  while (data->getSample(&data_input, &data_label) == true) {
    print_mutex->lock();
    printf("Data thread = %s", thread_id.c_str());
    for (size_type i = 0; i < data_input.size(); ++i) {
      if (data->format()[i] == kIndex) {
        const index_array field_index = ::std::get< index_array >(
            data_input[i]);
        printf(", data_input[%lu] = [%lu", i, field_index.size());
        for (size_type j = 0; j < field_index.size(); ++j) {
          printf(", %lu:%g", field_index[j].first, field_index[j].second);
        }
        printf("]");
      } else if (data->format()[i] == kBytes) {
        const byte_array field_bytes = ::std::get< byte_array >(data_input[i]);
        printf(", data_input[%lu] = [%lu, ", i, field_bytes.size());
        for (size_type j = 0; j < field_bytes.size(); ++j) {
          printf("%2.2x", field_bytes[j]);
        }
        printf("]");
      }
    }
    printf(", data_label = %lu:%g", data_label.first, data_label.second);
    printf("\n");
    print_mutex->unlock();
    for (size_type k = 0; k < universum_size; ++k) {
      EXPECT_TRUE(universum->getSample(
          input_size, label_size, data_input, data_label, &universum_input,
          &universum_label));
      print_mutex->lock();
      printf("Universum thread = %s", thread_id.c_str());
      for (size_type i = 0; i < universum_input.size(); ++i) {
        if (data->format()[i] == kIndex) {
          const index_array field_index = ::std::get< index_array >(
              universum_input[i]);
          printf(", data_input[%lu] = [%lu", i, field_index.size());
          for (size_type j = 0; j < field_index.size(); ++j) {
            printf(", %lu:%g", field_index[j].first, field_index[j].second);
          }
          printf("]");
        } else if (data->format()[i] == kBytes) {
          const byte_array &field_bytes = ::std::get< byte_array >(
              universum_input[i]);
          printf(", universum_input[%lu] = [%lu, ", i, field_bytes.size());
          for (size_type j = 0; j < field_bytes.size(); ++j) {
            printf("%2.2x", field_bytes[j]);
          }
          printf("]");
        }
      }
      printf(", universum_label = %lu:%g", universum_label.first,
             universum_label.second);
      printf("\n");
      print_mutex->unlock();
    }
  }
}

template < typename D, typename U >
void getSampleTest() {
  typedef typename D::format_array format_array;
  typedef typename U::size_storage size_storage;
  typedef typename U::size_type size_type;

  // Create data
  // Create data
  ::std::string file = "bytesteady/unittest_train.txt";
  format_array format = {kBytes, kIndex};
  D data(file, format);
  U universum;

  // Make the size storage
  size_type universum_size = 10;
  size_storage input_size = {64, 16};
  size_type label_size = 20;

  // Start the thread
  ::std::mutex print_mutex;
  size_type iteration_size = 3;
  size_type thread_size = 3;
  ::std::vector< ::std::thread > threads;
  for (int iteration = 0; iteration < iteration_size; ++iteration) {
    for (size_type i = 0; i < thread_size; ++i) {
      threads.push_back(::std::thread(
          getSampleThread< D, U >, input_size, label_size, universum_size,
          &data, &universum, &print_mutex));
    }
    for (size_type i = 0; i < thread_size; ++i) {
      threads[i].join();
    }
    threads.clear();
    EXPECT_EQ(20, data.count());
    EXPECT_TRUE(data.rewind());
  }
  EXPECT_EQ(iteration_size * universum_size * 20, universum.count());
}

TEST(DataTest, getSampleTest) {
  getSampleTest< DoubleData, DoubleUniversum >();
}

}  // namespace
}  // namespace bytesteady
