/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_HUFFMAN_CODEC_HPP_
#define BYTESTEADY_HUFFMAN_CODEC_HPP_

#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "bytesteady/bit_array.hpp"
#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"

namespace bytesteady {

class HuffmanCodec {
 public:
  typedef typename ::std::vector< uint8_t > byte_array;
  typedef typename byte_array::size_type size_type;
  typedef double value_type;
  typedef ::std::vector< size_type > size_array;
  typedef ::std::unordered_map< ::std::string, size_type > size_table;
  typedef ::std::unordered_map< ::std::string, byte_array > byte_table;
  typedef ::std::unordered_map< ::std::string, value_type > value_table;
  typedef ::std::function< bool (byte_array *input) > data_callback;

  // Node type for tree
  struct Node {
    ::std::string key;
    value_type value;
    ::std::shared_ptr< Node > left;
    ::std::shared_ptr< Node > right;
    
    bool operator>(const Node &b) const;
  };
  typedef Node node_type;
  // This is a min heap for Node
  typedef ::std::priority_queue<
    Node, ::std::vector< Node >, ::std::greater< Node > > node_queue;

  // Construct the codec
  HuffmanCodec(const size_array &g = {1});

  // Build the codec from a callback
  void build(const data_callback &callback);
  // Encode the data
  void encode(const byte_array &input, byte_array *output) const;
  // Decode the data
  void decode(const byte_array &input, byte_array *output) const;

  // Internal logic
  void buildFrequencyFromData(
      const data_callback &callback, value_table *frequency);
  void buildTreeFromFrequency(
      const value_table &frequency, Node *tree);
  void buildTableFromTree(
      const Node &tree, byte_table *table);
  void buildTreeFromTable(
      const value_table &frequency, const byte_table &table,
      Node *tree);
  void buildTableFromTreeRecursion(
      const byte_array &prefix, const Node &tree, byte_table *table);
  void encodeSingleGramLength(
      const byte_array &input, byte_array *output, size_type gram_length) const;
  void encodeMultiGramLength(
      const byte_array &input, byte_array *output, size_type min_gram,
      size_type max_gram) const;

  const size_array &gram() const;
  void set_gram(const size_array &g);

  const value_table &frequency() const;
  const byte_table &table() const;
  void set(const value_table &frequency, const byte_table &table);

 private:
  size_array gram_;
  value_table frequency_;
  byte_table table_;
  Node tree_;
};

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template < typename S >
void save(S *s, const ::bytesteady::HuffmanCodec &c);

template < typename S >
void load(S *s, ::bytesteady::HuffmanCodec *c);

}  // namespace serializer
}  // namespace thunder

namespace thunder {
namespace serializer {

// Pre-compiled template serializer instantiation
extern template void save(
    StringBinarySerializer *s, const ::bytesteady::HuffmanCodec &c);
extern template void StringBinarySerializer::save(
    const ::bytesteady::HuffmanCodec &c);
extern template void load(
    StringBinarySerializer *s, ::bytesteady::HuffmanCodec *c);
extern template void StringBinarySerializer::load(
    ::bytesteady::HuffmanCodec *c);
extern template void save(
    FileBinarySerializer *s, const ::bytesteady::HuffmanCodec &c);
extern template void FileBinarySerializer::save(
    const ::bytesteady::HuffmanCodec &c);
extern template void load(
    FileBinarySerializer *s, ::bytesteady::HuffmanCodec *c);
extern template void FileBinarySerializer::load(
    ::bytesteady::HuffmanCodec *c);
extern template void save(
    StringTextSerializer *s, const ::bytesteady::HuffmanCodec &c);
extern template void StringTextSerializer::save(
    const ::bytesteady::HuffmanCodec &c);
extern template void load(
    StringTextSerializer *s, ::bytesteady::HuffmanCodec *c);
extern template void StringTextSerializer::load(
    ::bytesteady::HuffmanCodec *c);
extern template void save(
    FileTextSerializer *s, const ::bytesteady::HuffmanCodec &c);
extern template void FileTextSerializer::save(
    const ::bytesteady::HuffmanCodec &c);
extern template void load(
    FileTextSerializer *s, ::bytesteady::HuffmanCodec *c);
extern template void FileTextSerializer::load(
    ::bytesteady::HuffmanCodec *c);

}  // namespace thunder
}  // namespace serializer

#endif  // BYTESTEADY_HUFFMAN_CODEC_HPP_
