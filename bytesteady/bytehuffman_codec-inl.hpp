/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/bytehuffman_codec.hpp"

#include <string>

#include "thunder/serializer.hpp"

namespace thunder {
namespace serializer {

template < typename S >
void save(S *s, const ::bytesteady::BytehuffmanCodec &c) {
  typedef ::bytesteady::BytehuffmanCodec BytehuffmanCodec;
  typedef typename BytehuffmanCodec::byte_array byte_array;
  typedef typename BytehuffmanCodec::byte_array_table byte_array_table;
  typedef typename BytehuffmanCodec::value_table value_table;

  // Save the gram configuration
  s->save(c.gram());

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

  // Save the encoder code
  const byte_array_table &code = c.code();
  s->save(code.size());
  for (const typename byte_array_table::value_type &pair : code) {
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
void load(S *s, ::bytesteady::BytehuffmanCodec *c) {
  typedef ::bytesteady::BytehuffmanCodec BytehuffmanCodec;
  typedef typename BytehuffmanCodec::size_type size_type;
  typedef typename BytehuffmanCodec::value_type value_type;
  typedef typename BytehuffmanCodec::byte_array byte_array;
  typedef typename BytehuffmanCodec::byte_array_table byte_array_table;
  typedef typename BytehuffmanCodec::value_table value_table;

  // Load gram configuration
  size_type gram;
  s->load(&gram);
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
  byte_array_table code;
  typename byte_array_table::size_type code_size;
  s->load(&code_size);
  for (typename byte_array_table::size_type i = 0; i < code_size; ++i) {
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
    code[key] = value;
  }

  c->set(frequency, code);
}

}  // namespace serializer
}  // namespace thunder
