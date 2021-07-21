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
