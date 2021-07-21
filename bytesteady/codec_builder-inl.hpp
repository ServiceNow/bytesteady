/*
 * Copyright 2019 XIang Zhang All Rights Reserved
 */

#include "bytesteady/codec_builder.hpp"

#include <variant>

namespace bytesteady {

template < typename D, typename C >
CodecBuilder< D, C >::CodecBuilder(
    D *d, codec_table *c, const gram_array &g) :
    data_(d), codec_(c), gram_(g) {}

template < typename D, typename C >
bool CodecBuilder< D, C >::build(
    const callback_type &callback) {
  Local local;
  field_array &data_input = local.data_input;
  index_pair &data_label = local.data_label;
  size_type &data_field = local.data_field;

  data_callback codec_callback =
      [&](byte_array *input) -> bool {
        if (data_->getSample(&data_input, &data_label)) {
          *input = ::std::get< byte_array >(data_input[data_field]);
          callback(local);
          return true;
        }
        return false;
      };
  codec_->clear();
  for (size_type i = 0; i < gram_.size(); ++i) {
    if (gram_[i].size() > 0) {
      data_field = i;
      if (data_->rewind() == false) {
        return false;
      }
      (*codec_)[i] = C(gram_[i]);
      (*codec_)[i].build(codec_callback);
    }
  }
  return true;
}

template < typename D, typename C >
const typename CodecBuilder< D, C >::gram_array & CodecBuilder< D, C >::gram() {
  return gram_;
}

}
