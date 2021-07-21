/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/huffman_codec.hpp"

#include <stdio.h>
#include <string>
#include <queue>
#include <vector>

#include "gtest/gtest.h"
#include "thunder/serializer.hpp"

#include "bytesteady/integer.hpp"

namespace bytesteady {
namespace {

TEST(HuffmanCodecTest, constructorTest) {
  typedef typename HuffmanCodec::size_array size_array;

  HuffmanCodec codec({1,2,4});
  const size_array &gram = codec.gram();
  EXPECT_EQ(3, gram.size());
  EXPECT_EQ(1, gram[0]);
  EXPECT_EQ(2, gram[1]);
  EXPECT_EQ(4, gram[2]);
}

TEST(HuffmanCodecTest, buildTreesAndTablesTest) {
  typedef typename HuffmanCodec::byte_array byte_array;
  typedef typename HuffmanCodec::byte_table byte_table;
  typedef typename HuffmanCodec::value_table value_table;
  typedef typename HuffmanCodec::data_callback data_callback;
  typedef typename HuffmanCodec::node_type node_type;

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
  HuffmanCodec codec({1,2,4});
  codec.buildFrequencyFromData(callback, &frequency);
  // TODO (xiang): check that the frequency table is okay.
  // for (const typename value_table::value_type &pair : frequency) {
    // printf("[%s]: %g\n", pair.first.c_str(), pair.second);
  // }

  // Build the decoder tree
  node_type tree{::std::string(), 0.0, nullptr, nullptr};
  codec.buildTreeFromFrequency(frequency, &tree);
  ::std::queue< const node_type* > queue;
  queue.push(&tree);
  // Breadth-first order
  while (!queue.empty()) {
    const node_type *node = queue.front();
    queue.pop();
    if (node->left.get() != nullptr) {
      queue.push(node->left.get());
    }
    if (node->right.get() != nullptr) {
      queue.push(node->right.get());
    }
    // TODO (xiang): check that the current node is okay.
    // printf("[%s]: %g\n", node->key.c_str(), node->value);
  }

  // Build the encoder table
  byte_table table;
  codec.buildTableFromTree(tree, &table);
  // TODO (xiang): check that the byte table is okay.
  // for (const typename byte_table::value_type &pair : table) {
    // printf("[%s]: ", pair.first.c_str());
    // for (const uint8_t bit : pair.second) {
    //   printf("%d", bit);
    // }
    // printf("\n");
  // }

  // Build decoder tree from the encoder table
  node_type second_tree;
  codec.buildTreeFromTable(frequency, table, &second_tree);
  ::std::queue< const node_type *> first_queue;
  ::std::queue< const node_type *> second_queue;
  first_queue.push(&tree);
  second_queue.push(&second_tree);
  while (!first_queue.empty()) {
    const node_type *first_node = first_queue.front();
    first_queue.pop();
    const node_type *second_node = second_queue.front();
    second_queue.pop();
    if (first_node->left.get() != nullptr) {
      first_queue.push(first_node->left.get());
      ASSERT_NE(nullptr, second_node->left.get());
      second_queue.push(second_node->left.get());
    } else {
      EXPECT_EQ(nullptr, second_node->left.get());
    }
    if (first_node->right.get() != nullptr) {
      first_queue.push(first_node->right.get());
      ASSERT_NE(nullptr, second_node->right.get());
      second_queue.push(second_node->right.get());
    } else {
      EXPECT_EQ(nullptr, second_node->right.get());
    }
    EXPECT_EQ(first_node->key, second_node->key);
    EXPECT_FLOAT_EQ(first_node->value, second_node->value);
  }
}

TEST(HuffmanCodecTest, encodeDecodeTest) {
  typedef typename HuffmanCodec::byte_array byte_array;
  typedef typename HuffmanCodec::data_callback data_callback;
  typedef typename HuffmanCodec::size_type size_type;

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
  HuffmanCodec codec({1,2,4});
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

TEST(HuffmanCodecTest, saveLoadTest) {
  typedef typename HuffmanCodec::byte_array byte_array;
  typedef typename HuffmanCodec::byte_table byte_table;
  typedef typename HuffmanCodec::data_callback data_callback;
  typedef typename HuffmanCodec::size_array size_array;
  typedef typename HuffmanCodec::value_table value_table;

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
  HuffmanCodec codec1({1,2,4});
  codec1.build(callback);

  // Serialize it
  ::thunder::StringBinarySerializer serializer;
  serializer.save(codec1);
  // Create another codec
  HuffmanCodec codec2({1,2});
  codec2.build(callback);
  // Load codec
  serializer.load(&codec2);

  // Check the gram
  const size_array &gram1 = codec1.gram();
  const size_array &gram2 = codec2.gram();
  ASSERT_EQ(gram1.size(), gram2.size());
  for (typename size_array::size_type i = 0; i < gram1.size(); ++i) {
    EXPECT_EQ(gram1[i], gram2[i]);
  }

  // Check the frequency
  const value_table &frequency1 = codec1.frequency();
  const value_table &frequency2 = codec2.frequency();
  EXPECT_EQ(frequency1.size(), frequency2.size());
  for (const typename value_table::value_type &pair : frequency1) {
    ASSERT_NE(frequency2.end(), frequency2.find(pair.first));
    EXPECT_FLOAT_EQ(pair.second, frequency2.at(pair.first));
  }

  // Check the encoder table
  const byte_table &table1 = codec1.table();
  const byte_table &table2 = codec2.table();
  EXPECT_EQ(table1.size(), table2.size());
  for (const typename byte_table::value_type &pair : table1) {
    ASSERT_NE(table2.end(), table2.find(pair.first));
    const byte_array &bytes1 = pair.second;
    const byte_array &bytes2 = table2.at(pair.first);
    ASSERT_EQ(bytes1.size(), bytes2.size());
    for (typename byte_array::size_type i = 0; i < bytes1.size(); ++i) {
      EXPECT_EQ(bytes1[i], bytes2[i]);
    }
  }
}

}  // namespace
}  // namespace bytesteady
