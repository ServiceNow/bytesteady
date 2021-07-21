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

#ifndef BYTESTEADY_BYTEHUFFMAN_CODEC_HPP_
#define BYTESTEADY_BYTEHUFFMAN_CODEC_HPP_

#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"

namespace bytesteady {

class BytehuffmanCodec {
 public:
  typedef typename ::std::vector< uint8_t > byte_array;
  typedef typename byte_array::size_type size_type;
  typedef double value_type;
  typedef ::std::vector< size_type > size_array;
  typedef ::std::unordered_map< ::std::string, size_type > size_table;
  typedef ::std::unordered_map< ::std::string, byte_array > byte_array_table;
  typedef ::std::unordered_map< ::std::string, value_type > value_table;
  typedef ::std::function< bool (byte_array *input) > data_callback;

  // Node type for tree
  struct Node {
    ::std::string key;
    value_type value;
    ::std::vector< Node > children;

    bool operator>(const Node &b) const;
  };
  typedef Node node_type;
  // This is a min heap for Node
  typedef ::std::priority_queue<
    Node, ::std::vector< Node >, ::std::greater< Node > > node_queue;

  // Construct the codec
  BytehuffmanCodec(const size_array &g = {1});

  // Build the codec from a callback
  void build(const data_callback &callback);
  // Encode the data
  void encode(const byte_array &input, byte_array *output) const;
  // Decode the data
  void decode(const byte_array &input, byte_array *output) const;

  // Internal logic
  void buildFrequencyFromData(
      const data_callback &callback, value_table *frequency) const;
  void buildTreeFromFrequency(
      const value_table &frequency, Node *tree) const;
  void buildCodeFromTree(
      const Node &tree, byte_array_table *code) const;
  void buildTreeFromCode(
      const value_table &frequency, const byte_array_table &code,
      Node *tree) const;

  size_type gram() const;
  void set_gram(size_type g);

  const value_table &frequency() const;
  const byte_array_table &code() const;
  void set(const value_table &f, const byte_array_table &c);

 private:
  size_type gram_;
  value_table frequency_;
  byte_array_table code_;
  Node tree_;
};

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template < typename S >
void save(S *s, const ::bytesteady::BytehuffmanCodec &c);

template < typename S >
void load(S *s, ::bytesteady::BytehuffmanCodec *c);

}  // namespace serializer
}  // namespace thunder

namespace thunder {
namespace serializer {

// Pre-compiled template serializer instantiation
extern template void save(
    StringBinarySerializer *s, const ::bytesteady::BytehuffmanCodec &c);
extern template void StringBinarySerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
extern template void load(
    StringBinarySerializer *s, ::bytesteady::BytehuffmanCodec *c);
extern template void StringBinarySerializer::load(
    ::bytesteady::BytehuffmanCodec *c);
extern template void save(
    FileBinarySerializer *s, const ::bytesteady::BytehuffmanCodec &c);
extern template void FileBinarySerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
extern template void load(
    FileBinarySerializer *s, ::bytesteady::BytehuffmanCodec *c);
extern template void FileBinarySerializer::load(
    ::bytesteady::BytehuffmanCodec *c);
extern template void save(
    StringTextSerializer *s, const ::bytesteady::BytehuffmanCodec &c);
extern template void StringTextSerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
extern template void load(
    StringTextSerializer *s, ::bytesteady::BytehuffmanCodec *c);
extern template void StringTextSerializer::load(
    ::bytesteady::BytehuffmanCodec *c);
extern template void save(
    FileTextSerializer *s, const ::bytesteady::BytehuffmanCodec &c);
extern template void FileTextSerializer::save(
    const ::bytesteady::BytehuffmanCodec &c);
extern template void load(
    FileTextSerializer *s, ::bytesteady::BytehuffmanCodec *c);
extern template void FileTextSerializer::load(
    ::bytesteady::BytehuffmanCodec *c);

}  // namespace thunder
}  // namespace serializer

#endif  // BYTESTEADY_BYTEHUFFMAN_CODEC_HPP_
