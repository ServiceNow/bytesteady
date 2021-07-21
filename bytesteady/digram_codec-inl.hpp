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
