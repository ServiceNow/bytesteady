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

#include "bytesteady/bytepair_codec.hpp"

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "thunder/serializer.hpp"

#include "bytesteady/integer.hpp"

namespace bytesteady {
namespace {

TEST(BytepairCodecTest, constructorTest) {
  BytepairCodec codec({2, 10});
  EXPECT_EQ(2, codec.gram());
}

TEST(BytepairCodecTest, buildTest) {
  typedef typename BytepairCodec::data_callback data_callback;
  typedef typename BytepairCodec::byte_array byte_array;
  typedef typename BytepairCodec::string_table string_table;
  typedef typename BytepairCodec::replace_pair replace_pair;

  // The data callback
  ::std::vector< ::std::string > data_array;
  data_array.push_back("hello world!");
  data_array.push_back("bytesteady");
  data_array.push_back("text classification and tagging");
  int n = 0;
  data_callback callback =
      [&](byte_array *input) -> bool {
        if (n >= data_array.size()) {
          n = 0;
          return false;
        }
        input->clear();
        for (const char &data_char : data_array[n]) {
          input->push_back(static_cast< uint8_t >(data_char));
        }
        n = n + 1;
        return true;
      };

  // The codec
  BytepairCodec codec({2, 10});

  // Build the replacement list
  codec.build(callback);
  // TODO(xiang): figure out a way to check the data according to the dataset
  for (const typename string_table::value_type &pair : codec.gram_text()) {
    printf("gram key = ");
    for (const char &c : pair.first) {
      printf("%.2x", static_cast< uint8_t >(c));
    }
    printf(", value = %s\n", pair.second.c_str());
  }
  for (const replace_pair &pair : codec.replace()) {
    printf("replace source = %s, target = ", pair.first.c_str());
    for (const char &c : pair.second) {
      printf("%.2x", static_cast< uint8_t >(c));
    }
    printf("\n");
  }
}

TEST(BytepairCodecTest, encodeDecodeTest) {
  typedef typename BytepairCodec::data_callback data_callback;
  typedef typename BytepairCodec::byte_array byte_array;

  // The data callback
  ::std::vector< ::std::string > data_array;
  data_array.push_back("hello world!");
  data_array.push_back("bytesteady");
  data_array.push_back("text classification and tagging");
  int n = 0;
  data_callback callback =
      [&](byte_array *input) -> bool {
        if (n >= data_array.size()) {
          n = 0;
          return false;
        }
        input->clear();
        for (const char &data_char : data_array[n]) {
          input->push_back(static_cast< uint8_t >(data_char));
        }
        n = n + 1;
        return true;
      };

  // The codec
  BytepairCodec codec({2});

  // Build the replacement list
  codec.build(callback);

  // Try encode and decode, verify the data are correct
  byte_array input, encoded, decoded;
  while (callback(&input)) {
    codec.encode(input, &encoded);
    codec.decode(encoded, &decoded);
    EXPECT_EQ(input, decoded);
    printf("Input text = %s", ::std::string(
        reinterpret_cast< const char * >(&input[0]), input.size()).c_str());
    printf(", encoded = ");
    for (const uint8_t byte : encoded) {
      printf("%.2x", byte);
    }
    printf(", decoded = %s\n", ::std::string(
        reinterpret_cast< const char * >(&decoded[0]), decoded.size()).c_str());
  }
}

TEST(BytepairCodecTest, saveLoadTest) {
  typedef typename BytepairCodec::byte_array byte_array;
  typedef typename BytepairCodec::data_callback data_callback;
  typedef typename BytepairCodec::size_type size_type;
  typedef typename BytepairCodec::replace_array replace_array;

  // Build the encoder and decoder tables and trees.
  ::std::vector< ::std::string > data_array;
  data_array.push_back("hello world!");
  data_array.push_back("bytesteady");
  data_array.push_back("text classification and tagging");
  int n = 0;
  data_callback callback =
      [&](byte_array *input) -> bool {
        if (n >= data_array.size()) {
          n = 0;
          return false;
        }
        input->clear();
        for (const char &data_char : data_array[n]) {
          input->push_back(static_cast< uint8_t >(data_char));
        }
        n = n + 1;
        return true;
      };
  BytepairCodec codec1({2});
  codec1.build(callback);

  // Serialize it
  ::thunder::StringBinarySerializer serializer;
  serializer.save(codec1);
  // Create another codec
  BytepairCodec codec2({1,2});
  codec2.build(callback);
  // Load codec
  serializer.load(&codec2);

  // Check the gram
  ASSERT_EQ(codec1.gram(), codec2.gram());

  // Check the replace list
  const replace_array replace1 = codec1.replace();
  const replace_array replace2 = codec2.replace();
  ASSERT_EQ(replace1.size(), replace2.size());
  for (size_type i = 0; i < replace1.size(); ++i) {
    EXPECT_EQ(replace1[i].first, replace2[i].first);
    EXPECT_EQ(replace1[i].second, replace2[i].second);
  }

  // Check the encoded and decoded result
  byte_array input, encoded1, encoded2, decoded1, decoded2;
  while (callback(&input)) {
    codec1.encode(input, &encoded1);
    codec2.encode(input, &encoded2);
    EXPECT_EQ(encoded1, encoded2);
    codec1.decode(encoded1, &decoded1);
    codec2.decode(encoded1, &decoded2);
    EXPECT_EQ(input, decoded1);
    EXPECT_EQ(input, decoded2);
  }
}

}  // namespace
}  // namespace bytesteady
