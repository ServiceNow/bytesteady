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

#include "bytesteady/huffman_codec.hpp"

#include <string>

#include "thunder/serializer.hpp"

namespace thunder {
namespace serializer {

template < typename S >
void save(S *s, const ::bytesteady::HuffmanCodec &c) {
  typedef ::bytesteady::HuffmanCodec HuffmanCodec;
  typedef typename HuffmanCodec::byte_array byte_array;
  typedef typename HuffmanCodec::size_array size_array;
  typedef typename HuffmanCodec::byte_table byte_table;
  typedef typename HuffmanCodec::value_table value_table;

  // Save the gram configuration
  const size_array &gram = c.gram();
  s->save(gram.size());
  for (const typename size_array::value_type &g : gram) {
    s->save(g);
  }

  // Save the frequency table
  const value_table &frequency = c.frequency();
  s->save(frequency.size());
  for (const typename value_table::value_type &pair : frequency) {
    s->save(pair.first.size());
    for (const char &key_char : pair.first) {
      s->save(key_char);
    }
    s->save(pair.second);
  }

  // Save the encoder table
  const byte_table &table = c.table();
  s->save(table.size());
  for (const typename byte_table::value_type &pair : table) {
    s->save(pair.first.size());
    for (const char &key_char : pair.first) {
      s->save(key_char);
    }
    s->save(pair.second.size());
    for (const typename byte_array::value_type &value_byte : pair.second) {
      s->save(value_byte);
    }
  }
}

template < typename S >
void load(S *s, ::bytesteady::HuffmanCodec *c) {
  typedef ::bytesteady::HuffmanCodec HuffmanCodec;
  typedef typename HuffmanCodec::byte_array byte_array;
  typedef typename HuffmanCodec::value_type value_type;
  typedef typename HuffmanCodec::size_array size_array;
  typedef typename HuffmanCodec::byte_table byte_table;
  typedef typename HuffmanCodec::value_table value_table;

  // Load the gram configuration
  typename size_array::size_type gram_size;
  s->load(&gram_size);
  size_array gram(gram_size);
  for (typename size_array::size_type i = 0; i < gram_size; ++i) {
    s->load(&gram[i]);
  }
  c->set_gram(gram);

  // Load the frequency table
  value_table frequency;
  typename value_table::size_type frequency_size;
  s->load(&frequency_size);
  for (typename value_table::size_type i = 0; i < frequency_size; ++i) {
    typename ::std::string::size_type key_size;
    s->load(&key_size);
    ::std::string key(key_size, '\0');
    for (typename ::std::string::size_type i = 0; i < key_size; ++i) {
      s->load(&key[i]);
    }
    value_type value;
    s->load(&value);
    frequency[key] = value;
  }

  // Load the encoder table
  byte_table table;
  typename byte_table::size_type table_size;
  s->load(&table_size);
  for (typename byte_table::size_type i = 0; i < table_size; ++i) {
    typename ::std::string::size_type key_size;
    s->load(&key_size);
    ::std::string key(key_size, '\0');
    for (typename ::std::string::size_type i = 0; i < key_size; ++i) {
      s->load(&key[i]);
    }
    typename byte_array::size_type value_size;
    s->load(&value_size);
    byte_array value(value_size);
    for (typename byte_array::size_type i = 0; i < value_size; ++i) {
      s->load(&value[i]);
    }
    table[key] = value;
  }
  c->set(frequency, table);
}

}  // namespace serializer
}  // namespace thunder
