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

#ifndef BYTESTEADY_DIGRAM_CODEC_HPP_
#define BYTESTEADY_DIGRAM_CODEC_HPP_

#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "bytesteady/integer.hpp"
#include "thunder/serializer.hpp"

namespace bytesteady {

class DigramCodec {
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
  DigramCodec(const size_array &g = {});

  // Build the codec from a callback
  void build(const data_callback &callback);
  // Encode the data
  void encode(const byte_array &input, byte_array *output) const;
  // Decode the data
  void decode(const byte_array &input, byte_array *output) const;

  // Internal Logic
  void buildCountFromData(
      const data_callback &callback, size_table *count) const;
  void buildBaseGramSetFromCount(
      size_table *count, string_set *base_gram_set) const;
  void buildGramTextFromCount(
      size_table *count, string_set *base_gram_set,
      string_table *gram_text) const;
  void buildReplaceFromGramText(
      const string_table &gram_text, replace_array *replace) const;

  // Utility functions
  uint8_t increase(byte_array *key) const;
  void replaceString(
      const ::std::string &source, const ::std::string &target,
      ::std::string *output) const;

  size_type dict_size() const;
  void set_dict_size(size_type d);

  size_type gram_size() const;
  void set_gram_size(size_type g);

  size_type count_size() const;
  void set_count_size(size_type c);

  size_type count_threshold() const;
  void set_count_threshold(size_type t);

  const replace_array &replace() const;
  void set_replace(const replace_array &r);

  const size_table &count() const;
  const string_set &base_gram_set() const;
  const string_table &gram_text() const;

 private:
  // Size of the dictionary entry
  size_type dict_size_;
  // Maximum gram length to store in count table
  size_type gram_size_;
  // Maximum size of count table. If reached, will remove the count entry
  size_type count_size_;
  // Remove count whose size is less than this threshold
  size_type count_threshold_;

  // Store the count table
  size_table count_;
  // Store the set of base grams
  string_set base_gram_set_;
  // Store the gram and text correspondence
  string_table gram_text_;
  // Store the replace pairs for encoding and decoding
  replace_array replace_;
};

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template < typename S >
void save (S *s, const ::bytesteady::DigramCodec &c);

template < typename S >
void load (S *s, ::bytesteady::DigramCodec *c);

}  // namespace serializer
}  // namespace thunder

namespace thunder {
namespace serializer {

// Pre-compiled template serializer instantiation
extern template void save(
    StringBinarySerializer *s, const ::bytesteady::DigramCodec &c);
extern template void StringBinarySerializer::save(
    const ::bytesteady::DigramCodec &c);
extern template void load(
    StringBinarySerializer *s, ::bytesteady::DigramCodec *c);
extern template void StringBinarySerializer::load(
    ::bytesteady::DigramCodec *c);
extern template void save(
    FileBinarySerializer *s, const ::bytesteady::DigramCodec &c);
extern template void FileBinarySerializer::save(
    const ::bytesteady::DigramCodec &c);
extern template void load(
    FileBinarySerializer *s, ::bytesteady::DigramCodec *c);
extern template void FileBinarySerializer::load(
    ::bytesteady::DigramCodec *c);
extern template void save(
    StringTextSerializer *s, const ::bytesteady::DigramCodec &c);
extern template void StringTextSerializer::save(
    const ::bytesteady::DigramCodec &c);
extern template void load(
    StringTextSerializer *s, ::bytesteady::DigramCodec *c);
extern template void StringTextSerializer::load(
    ::bytesteady::DigramCodec *c);
extern template void save(
    FileTextSerializer *s, const ::bytesteady::DigramCodec &c);
extern template void FileTextSerializer::save(
    const ::bytesteady::DigramCodec &c);
extern template void load(
    FileTextSerializer *s, ::bytesteady::DigramCodec *c);
extern template void FileTextSerializer::load(
    ::bytesteady::DigramCodec *c);

}  // namespace thunder
}  // namespace serializer

#endif  // BYTESTEADY_DIGRAM_CODEC_HPP_
