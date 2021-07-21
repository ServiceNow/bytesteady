/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/bit_array.hpp"

#include "gtest/gtest.h"

namespace bytesteady {
namespace {

TEST(BitArrayTest, pushPopTest) {
  BitArray array;

  array.pushFront(1);
  array.pushFront(0);
  array.pushFront(0);
  array.pushFront(0);
  array.pushFront(1);
  array.pushFront(0);
  // 010001
  EXPECT_EQ(6, array.size());
  EXPECT_EQ(0, array.front());
  EXPECT_EQ(1, array.back());

  array.pushBack(0);
  array.pushBack(1);
  array.pushBack(1);
  array.pushBack(0);
  array.pushBack(1);
  array.pushBack(1);
  // 010001011011
  EXPECT_EQ(12, array.size());
  EXPECT_EQ(0, array.front());
  EXPECT_EQ(1, array.back());

  // 010001011011
  EXPECT_EQ(0, array.popFront());
  // 10001011011
  EXPECT_EQ(11, array.size());
  EXPECT_EQ(1, array.popFront());
  // 0001011011
  EXPECT_EQ(10, array.size());
  EXPECT_EQ(1, array.popBack());
  // 000101101
  EXPECT_EQ(9, array.size());
  EXPECT_EQ(1, array.popBack());
  // 00010110
  EXPECT_EQ(8, array.size());
  EXPECT_EQ(0, array.back());

  array.clear();
  EXPECT_EQ(0, array.size());
}

TEST(BitArrayTest, getPaddedArrayTest) {
  typedef typename BitArray::byte_array byte_array;

  BitArray bits;
  bits.pushFront(1);
  bits.pushFront(0);
  bits.pushFront(0);
  bits.pushFront(1);
  bits.pushFront(1);
  bits.pushFront(0);
  EXPECT_EQ(6, bits.size());
  // 011001--
  byte_array bytes = bits.getPaddedBytes(0);
  EXPECT_EQ(1, bytes.size());
  EXPECT_EQ(0b01100100, bytes[0]);

  bits.pushFront(1);
  bits.pushFront(0);
  bits.pushFront(1);
  bits.pushFront(1);
  EXPECT_EQ(10, bits.size());
  // 11-01011001-
  bytes = bits.getPaddedBytes(1);
  EXPECT_EQ(2, bytes.size());
  EXPECT_EQ(0b11010110, bytes[0]);
  EXPECT_EQ(0b01111111, bytes[1]);

  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(1);
  bits.pushBack(0);
  EXPECT_EQ(14, bits.size());
  // 11-01011001-0110
  bytes = bits.getPaddedBytes(0);
  EXPECT_EQ(2, bytes.size());
  EXPECT_EQ(0b11010110, bytes[0]);
  EXPECT_EQ(0b01011000, bytes[1]);

  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(0);
  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(0);
  EXPECT_EQ(20, bits.size());
  // 11-01011001 01100100-10
  bytes = bits.getPaddedBytes(0);
  EXPECT_EQ(3, bytes.size());
  EXPECT_EQ(0b11010110, bytes[0]);
  EXPECT_EQ(0b01011001, bytes[1]);
  EXPECT_EQ(0b00100000, bytes[2]);

  bits.clear();
  EXPECT_EQ(0, bits.size());
  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(0);
  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(0);
  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(1);
  bits.pushBack(0);
  EXPECT_EQ(10, bits.size());
  // -01001001-10
  bytes = bits.getPaddedBytes(0);
  EXPECT_EQ(2, bytes.size());
  EXPECT_EQ(0b01001001, bytes[0]);
  EXPECT_EQ(0b10000000, bytes[1]);

  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(1);
  bits.pushBack(1);
  bits.pushBack(1);
  bits.pushBack(0);
  bits.pushBack(0);
  bits.pushBack(0);
  bits.pushBack(1);
  bits.pushBack(0);
  EXPECT_EQ(20, bits.size());
  // -01001001 10011110-0010
  bytes = bits.getPaddedBytes(0);
  EXPECT_EQ(3, bytes.size());
  EXPECT_EQ(0b01001001, bytes[0]);
  EXPECT_EQ(0b10011110, bytes[1]);
  EXPECT_EQ(0b00100000, bytes[2]);

  bits.pushFront(0);
  bits.pushFront(1);
  bits.pushFront(0);
  bits.pushFront(0);
  bits.pushFront(1);
  bits.pushFront(0);
  bits.pushFront(1);
  bits.pushFront(1);
  EXPECT_EQ(28, bits.size());
  // 11010010-01001001 10011110-0010
  bytes = bits.getPaddedBytes(0);
  EXPECT_EQ(4, bytes.size());
  EXPECT_EQ(0b11010010, bytes[0]);
  EXPECT_EQ(0b01001001, bytes[1]);
  EXPECT_EQ(0b10011110, bytes[2]);
  EXPECT_EQ(0b00100000, bytes[3]);
}

}
}  // namespace bytesteady
