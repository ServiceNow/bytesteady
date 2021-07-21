/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/digram_codec.hpp"

#include <string>

#include "thunder/serializer.hpp"

namespace thunder {
namespace serializer {

template < typename S >
void save(S *s, const ::bytesteady::DigramCodec &c) {
  typedef ::bytesteady::DigramCodec DigramCodec;
  typedef typename DigramCodec::size_type size_type;
  typedef typename DigramCodec::replace_pair replace_pair;
  typedef typename DigramCodec::replace_array replace_array;

  // Save the gram configuration
  size_type dict_size = c.dict_size();
  s->save(dict_size);
  size_type gram_size = c.gram_size();
  s->save(gram_size);

  // Save the replace array
  const replace_array &replace = c.replace();
  s->save(replace.size());
  for (const replace_pair &pair : replace) {
    s->save(pair.first.size());
    for (const char &c : pair.first) {
      s->save(c);
    }
    s->save(pair.second.size());
    for (const char &c : pair.second) {
      s->save(c);
    }
  }
}

template < typename S >
void load(S *s, ::bytesteady::DigramCodec *c) {
  typedef ::bytesteady::DigramCodec DigramCodec;
  typedef typename DigramCodec::size_type size_type;
  typedef typename DigramCodec::replace_array replace_array;

  size_type dict_size;
  s->load(&dict_size);
  c->set_dict_size(dict_size);

  size_type gram_size;
  s->load(&gram_size);
  c->set_gram_size(gram_size);

  // Load the replace array
  replace_array replace;
  typename replace_array::size_type replace_size;
  s->load(&replace_size);
  for (size_type i = 0; i < replace_size; ++i) {
    typename ::std::string::size_type source_size;
    s->load(&source_size);
    ::std::string source(source_size, '\0');
    for (typename ::std::string::size_type i = 0; i < source_size; ++i) {
      s->load(&source[i]);
    }
    typename ::std::string::size_type target_size;
    s->load(&target_size);
    ::std::string target(target_size, '\0');
    for (typename ::std::string::size_type i = 0; i < target_size; ++i) {
      s->load(&target[i]);
    }
    replace.push_back(::std::make_pair(source, target));
  }
  c->set_replace(replace);
}

}  // namespace serializer
}  // namespace thunder
