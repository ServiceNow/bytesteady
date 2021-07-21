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

#include "bytesteady/subsample_codec.hpp"

#include "thunder/serializer.hpp"

namespace thunder {
namespace serializer {

template < typename S >
void save(S *s, const ::bytesteady::SubsampleCodec &c) {
  s->save(c.factor());
}

template < typename S >
void load(S *s, ::bytesteady::SubsampleCodec *c) {
  typedef typename ::bytesteady::SubsampleCodec::size_type size_type;
  size_type factor;
  s->load(&factor);
  c->set_factor(factor);
}

}  // namespace serializer
}  // namespace thunder
