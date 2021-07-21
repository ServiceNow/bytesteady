/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/digram_codec.hpp"

#include <limits>

#include "bytesteady/integer.hpp"
#include "gtest/gtest.h"

namespace bytesteady {
namespace {

TEST(DigramCodecTest, constructorTest) {
  DigramCodec codec({2, 64, 100, 5});
  EXPECT_EQ(2, codec.dict_size());
  EXPECT_EQ(64, codec.gram_size());
  EXPECT_EQ(100, codec.count_size());
  EXPECT_EQ(5, codec.count_threshold());
}

TEST(DigramCodecTest, buildTest) {
  typedef typename DigramCodec::data_callback data_callback;
  typedef typename DigramCodec::byte_array byte_array;
  typedef typename DigramCodec::replace_array replace_array;

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
  DigramCodec codec({2, 8});

  // Build replace array
  codec.build(callback);
  // TODO(xiang): figure out a way to check the replace list
  const replace_array &replace = codec.replace();
  for (const typename replace_array::value_type pair : replace) {
    printf("Replace source = %s, target = ", pair.first.c_str());
    for (const char &target_byte : pair.second) {
      printf("%.2x", static_cast< uint8_t >(target_byte));
    }
    printf("\n");
  }
}

TEST(DigramCodecTest, encodeDecodeTest) {
  typedef typename DigramCodec::data_callback data_callback;
  typedef typename DigramCodec::byte_array byte_array;

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
  DigramCodec codec({2, 8});

  // Build the replacement list
  codec.build(callback);

  // Try encode and decode, verify the data are correct
  byte_array input, encoded, decoded;
  while (callback(&input)) {
    codec.encode(input, &encoded);
    codec.decode(encoded, &decoded);
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

TEST(DigramCodecTest, saveLoadTest) {
  typedef typename DigramCodec::byte_array byte_array;
  typedef typename DigramCodec::data_callback data_callback;
  typedef typename DigramCodec::size_type size_type;
  typedef typename DigramCodec::replace_array replace_array;

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
  DigramCodec codec1({2, 8});
  codec1.build(callback);

  // Serialize it
  ::thunder::StringBinarySerializer serializer;
  serializer.save(codec1);
  // Create another codec
  DigramCodec codec2({1, 2});
  codec2.build(callback);
  // Load codec
  serializer.load(&codec2);

  // Check the dict size and gram size
  ASSERT_EQ(codec1.dict_size(), codec2.dict_size());
  ASSERT_EQ(codec1.gram_size(), codec2.gram_size());

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

