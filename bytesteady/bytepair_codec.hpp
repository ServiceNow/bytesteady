/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_BYTEPAIR_CODEC_HPP_
#define BYTESTEADY_BYTEPAIR_CODEC_HPP_

#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"

namespace bytesteady {

class BytepairCodec {
 public:
  typedef ::std::vector< uint8_t > byte_array;
  typedef typename byte_array::size_type size_type;
  typedef ::std::vector< size_type > size_array;
  typedef ::std::vector< ::std::string > string_array;
  typedef ::std::unordered_map< ::std::string, size_type > size_table;
  typedef ::std::unordered_map< ::std::string, ::std::string > string_table;
  typedef ::std::unordered_set< ::std::string > string_set;
  typedef ::std::pair< ::std::string, ::std::string > replace_pair;
  typedef ::std::vector< replace_pair > replace_array;
  typedef ::std::function< bool (byte_array *input) > data_callback;

  // Node type for priority queue
  struct Node {
    ::std::string target;
    ::std::string source;
    size_type value;

    bool operator<(const Node &b) const;
  };
  typedef Node node_type;
  // This is a max heap for Node
  typedef ::std::priority_queue<
    Node, ::std::vector< Node >, ::std::less< Node > > node_queue;

  // Construct the codec
  BytepairCodec(const size_array &g = {});

  // Build the codec from a callback
  void build(const data_callback &callback);
  // Encode the data
  void encode(const byte_array &input, byte_array *output) const;
  // Decode the data
  void decode(const byte_array &input, byte_array *output) const;

  // Internal Logic
  void buildDataFromCallback(
      const data_callback &callback, string_array *data) const;
  void buildGramTextFromData(
      string_array *data, string_table *gram_text) const;
  void buildReplaceFromGramText(
      const string_table &gram_text, replace_array *replace) const;

  // Utility functions
  uint8_t increase(byte_array *key) const;
  void replaceString(
      const ::std::string &source, const ::std::string &target,
      ::std::string *output) const;

  // Multi-threaded replace and count
  void replaceAll(
      const ::std::string &source, const ::std::string &target,
      string_array *data) const;
  void replaceAllJob(
      const ::std::string &source, const ::std::string &target,
      string_array *data, size_type *index, ::std::mutex *mutex) const;
  void countAll(
      const string_array &data, string_set *gram_set,
      size_table *digram_count) const;
  void countAllJob(
      const string_array &data, string_set *gram_set,
      size_table *digram_count, size_type *index, ::std::mutex *mutex) const;

  size_type gram() const;
  void set_gram(size_type g);

  const replace_array &replace() const;
  void set_replace(const replace_array &r);

  const string_array &data() const;
  const string_table &gram_text() const;

 private:
  size_type gram_;
  size_type thread_size_;

  // Store the data for the algorithm
  string_array data_;
  // Store the gram and text correspondence
  string_table gram_text_;
  // Store the replace pairs for encoding and decoding
  replace_array replace_;
};

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template < typename S >
void save (S *s, const ::bytesteady::BytepairCodec &c);

template < typename S >
void load (S *s, ::bytesteady::BytepairCodec *c);

}  // namespace serializer
}  // namespace thunder

namespace thunder {
namespace serializer {

// Pre-compiled template serializer instantiation
extern template void save(
    StringBinarySerializer *s, const ::bytesteady::BytepairCodec &c);
extern template void StringBinarySerializer::save(
    const ::bytesteady::BytepairCodec &c);
extern template void load(
    StringBinarySerializer *s, ::bytesteady::BytepairCodec *c);
extern template void StringBinarySerializer::load(
    ::bytesteady::BytepairCodec *c);
extern template void save(
    FileBinarySerializer *s, const ::bytesteady::BytepairCodec &c);
extern template void FileBinarySerializer::save(
    const ::bytesteady::BytepairCodec &c);
extern template void load(
    FileBinarySerializer *s, ::bytesteady::BytepairCodec *c);
extern template void FileBinarySerializer::load(
    ::bytesteady::BytepairCodec *c);
extern template void save(
    StringTextSerializer *s, const ::bytesteady::BytepairCodec &c);
extern template void StringTextSerializer::save(
    const ::bytesteady::BytepairCodec &c);
extern template void load(
    StringTextSerializer *s, ::bytesteady::BytepairCodec *c);
extern template void StringTextSerializer::load(
    ::bytesteady::BytepairCodec *c);
extern template void save(
    FileTextSerializer *s, const ::bytesteady::BytepairCodec &c);
extern template void FileTextSerializer::save(
    const ::bytesteady::BytepairCodec &c);
extern template void load(
    FileTextSerializer *s, ::bytesteady::BytepairCodec *c);
extern template void FileTextSerializer::load(
    ::bytesteady::BytepairCodec *c);

}  // namespace thunder
}  // namespace serializer

#endif  // BYTESTEADY_BYTEPAIR_CODEC_HPP_
