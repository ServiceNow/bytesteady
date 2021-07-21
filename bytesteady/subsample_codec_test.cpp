/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/subsample_codec.hpp"

#include <vector>

#include "gtest/gtest.h"
#include "thunder/serializer.hpp"

#include "bytesteady/integer.hpp"

namespace bytesteady {
namespace {

TEST(SubsampleCodecTest, constructorTest) {
  SubsampleCodec codec({2});
  EXPECT_EQ(2, codec.factor());
}

TEST(SubsampleCodecTest, encodeDecodeTest) {
  typedef typename SubsampleCodec::byte_array byte_array;
  typedef typename SubsampleCodec::data_callback data_callback;
  typedef typename SubsampleCodec::size_type size_type;

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
  SubsampleCodec codec({4});
  codec.build(callback);

  // Test for data in the dictionary
  for (const ::std::string &data_string : data_array) {
    byte_array data_input;
    for (const char &data_char : data_string) {
      data_input.push_back(static_cast< const uint8_t >(data_char));
    }
    byte_array data_encoded;
    codec.encode(data_input, &data_encoded);
    byte_array data_decoded;
    codec.decode(data_encoded, &data_decoded);
    EXPECT_EQ(data_input.size(), data_encoded.size());
    EXPECT_EQ(data_input.size(), data_decoded.size());
    for (size_type i = 0; i < data_input.size(); ++i) {
      EXPECT_EQ(data_input[i] / 4, data_encoded[i]);
      EXPECT_EQ(data_input[i] / 4 * 4, data_decoded[i]);
    }
  }

  // Test for data outside of the dictionary
  ::std::string data_string("A quick brown fox jumps over the lazy dog.");
  byte_array data_input;
  for (const char &data_char : data_string) {
    data_input.push_back(static_cast< const uint8_t >(data_char));
  }
  byte_array data_encoded;
  codec.encode(data_input, &data_encoded);
  byte_array data_decoded;
  codec.decode(data_encoded, &data_decoded);
  EXPECT_EQ(data_input.size(), data_encoded.size());
  EXPECT_EQ(data_input.size(), data_decoded.size());
  for (size_type i = 0; i < data_input.size(); ++i) {
    EXPECT_EQ(data_input[i] / 4, data_encoded[i]);
    EXPECT_EQ(data_input[i] / 4 * 4, data_decoded[i]);
  }
}

TEST(SubsampleCodecTest, saveLoadTest) {
  typedef typename SubsampleCodec::byte_array byte_array;
  typedef typename SubsampleCodec::data_callback data_callback;
  typedef typename SubsampleCodec::size_type size_type;

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
  SubsampleCodec codec1({4});
  codec1.build(callback);

  // Serialize it
  ::thunder::StringBinarySerializer serializer;
  serializer.save(codec1);
  // Create another codec
  SubsampleCodec codec2({8});
  // Load codec
  serializer.load(&codec2);

  // Check the factor
  EXPECT_EQ(codec1.factor(), codec2.factor());
}

}  // namespace
}  // namespace bytesteady
