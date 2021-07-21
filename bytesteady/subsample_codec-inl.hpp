/*
 * Copyright 2020 Xiang Zhang All Rights Reserved.
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
