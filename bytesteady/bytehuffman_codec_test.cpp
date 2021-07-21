/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/bytehuffman_codec.hpp"

#include <stdio.h>
#include <string>
#include <queue>
#include <vector>

#include "gtest/gtest.h"
#include "thunder/serializer.hpp"

#include "bytesteady/integer.hpp"

namespace bytesteady {
namespace {

TEST(BytehuffmanCodecTest, constructorTest) {
  BytehuffmanCodec codec({2});
  EXPECT_EQ(2, codec.gram());
}

TEST(BytehuffmanCodecTest, buildTest) {
  typedef typename BytehuffmanCodec::size_type size_type;
  typedef typename BytehuffmanCodec::byte_array byte_array;
  typedef typename BytehuffmanCodec::data_callback data_callback;
  typedef typename BytehuffmanCodec::value_table value_table;
  typedef typename BytehuffmanCodec::byte_array_table byte_array_table;
  typedef typename BytehuffmanCodec::node_type node_type;

    // Build the frequency table
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
  value_table frequency;
  BytehuffmanCodec codec({2});
  codec.buildFrequencyFromData(callback, &frequency);
  // TODO(xiang): check that the frequency table is okay
  for (const typename value_table::value_type &pair : frequency) {
    printf("Frequency key = %s, value = %g\n", pair.first.c_str(), pair.second);
  }

  // Build the decoder tree
  node_type tree;
  codec.buildTreeFromFrequency(frequency, &tree);
  // TODO (xiang): check that the tree is okay
  ::std::queue< const node_type* > queue;
  queue.push(&tree);
  while(!queue.empty()) {
    const node_type *node = queue.front();
    queue.pop();
    for (const node_type &child : node->children) {
      queue.push(&child);
    }
    printf("Tree key = %s, value = %g\n", node->key.c_str(), node->value);
  }

  // Build the encoder code table
  byte_array_table code;
  codec.buildCodeFromTree(tree, &code);
  // TODO (xiang): check that the code table is okay
  for (const typename byte_array_table::value_type &pair : code) {
    printf("Code key = %s, value = ", pair.first.c_str());
    for (const uint8_t &byte : pair.second) {
      printf("%.2x", byte);
    }
    printf("\n");
  }

  // Build decoder tree from the encoder code table
  node_type second_tree;
  codec.buildTreeFromCode(frequency, code, &second_tree);
  ::std::queue< const node_type * > first_queue;
  ::std::queue< const node_type * > second_queue;
  first_queue.push(&tree);
  second_queue.push(&second_tree);
  while (!first_queue.empty()) {
    const node_type *first_node = first_queue.front();
    first_queue.pop();
    const node_type *second_node = second_queue.front();
    second_queue.pop();
    ASSERT_EQ(first_node->children.size(), second_node->children.size());
    for (size_type i = 0; i < first_node->children.size(); ++i) {
      first_queue.push(&(first_node->children[i]));
      second_queue.push(&(second_node->children[i]));
    }
    EXPECT_EQ(first_node->key, second_node->key);
    EXPECT_FLOAT_EQ(first_node->value, second_node->value);
  }
}

TEST(BytehuffmanCodecTest, encodeDecodeTest) {
  typedef typename BytehuffmanCodec::byte_array byte_array;
  typedef typename BytehuffmanCodec::data_callback data_callback;
  typedef typename BytehuffmanCodec::size_type size_type;

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
  BytehuffmanCodec codec({1,2,4});
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
    EXPECT_EQ(data_input.size(), data_decoded.size());
    for (size_type i = 0; i < data_input.size(); ++i) {
      EXPECT_EQ(data_input[i], data_decoded[i]);
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
  EXPECT_GE(data_input.size(), data_encoded.size());
  EXPECT_GE(data_input.size(), data_decoded.size());

  // Test that the decoded sequence is a subsequence of input
  size_type data_decoded_index = 0;
  for (const uint8_t byte : data_input) {
    ASSERT_GE(data_decoded.size(), data_decoded_index);
    if (data_decoded_index < data_decoded.size() &&
        byte == data_decoded[data_decoded_index]) {
      data_decoded_index = data_decoded_index + 1;
    }
  }
  EXPECT_EQ(data_decoded.size(), data_decoded_index);
}

TEST(BytehuffmanCodecTest, saveLoadTest) {
  typedef typename BytehuffmanCodec::byte_array byte_array;
  typedef typename BytehuffmanCodec::byte_array_table byte_array_table;
  typedef typename BytehuffmanCodec::data_callback data_callback;
  typedef typename BytehuffmanCodec::size_type size_type;
  typedef typename BytehuffmanCodec::value_table value_table;

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
  BytehuffmanCodec codec1({1,2,4});
  codec1.build(callback);

  // Serialize it
  ::thunder::StringBinarySerializer serializer;
  serializer.save(codec1);
  // Create another codec
  BytehuffmanCodec codec2({1,2});
  codec2.build(callback);
  // Load codec
  serializer.load(&codec2);

  // Check the gram
  const size_type gram1 = codec1.gram();
  const size_type gram2 = codec2.gram();
  EXPECT_EQ(gram1, gram2);

  // Check the frequency
  const value_table &frequency1 = codec1.frequency();
  const value_table &frequency2 = codec2.frequency();
  EXPECT_EQ(frequency1.size(), frequency2.size());
  for (const typename value_table::value_type &pair : frequency1) {
    ASSERT_NE(frequency2.end(), frequency2.find(pair.first));
    EXPECT_FLOAT_EQ(pair.second, frequency2.at(pair.first));
  }

  // Check the encoder table
  const byte_array_table &code1 = codec1.code();
  const byte_array_table &code2 = codec2.code();
  EXPECT_EQ(code1.size(), code2.size());
  for (const typename byte_array_table::value_type &pair : code1) {
    ASSERT_NE(code2.end(), code2.find(pair.first));
    const byte_array &bytes1 = pair.second;
    const byte_array &bytes2 = code2.at(pair.first);
    ASSERT_EQ(bytes1.size(), bytes2.size());
    for (typename byte_array::size_type i = 0; i < bytes1.size(); ++i) {
      EXPECT_EQ(bytes1[i], bytes2[i]);
    }
  }
}

}  // namespace
}  // namespace bytesteady
