/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/codec_builder.hpp"

#include <string>

#include "bytesteady/field_format.hpp"
#include "bytesteady/integer.hpp"
#include "gtest/gtest.h"

namespace bytesteady {
namespace {

template < typename C >
void buildTest() {
  typedef typename C::codec_table codec_table;
  typedef typename C::codec_type codec_type;
  typedef typename C::data_type data_type;
  typedef typename C::gram_array gram_array;
  typedef typename C::local_type builder_local;
  typedef typename C::callback_type builder_callback;
  typedef typename C::size_type size_type;
  typedef typename codec_type::value_table value_table;
  typedef typename data_type::byte_array byte_array;
  typedef typename data_type::field_array field_array;
  typedef typename data_type::format_array format_array;
  typedef typename data_type::index_array index_array;
  typedef typename data_type::index_pair index_pair;

  // Create data
  ::std::string file = "bytesteady/unittest_train.txt";
  format_array format = {kBytes, kIndex};
  data_type data(file, format);

  // Create codec table
  codec_table codec;

  // Create gram array
  gram_array gram = {{1,2,4},{}};

  // Create the codec builder
  C builder(&data, &codec, gram);

  // The callback
  builder_callback callback =
      [&](const builder_local &local) -> void {
        printf("Build count = %lu", data.count());
        const field_array &data_input = local.data_input;
        const index_pair &data_label = local.data_label;
        const size_type &data_field = local.data_field;
        for (size_type i = 0; i < data_input.size(); ++i) {
          if (data.format()[i] == kIndex) {
            const index_array field_index = ::std::get< index_array >(
                data_input[i]);
            printf(", data_input[%lu] = [%lu", i, field_index.size());
            for (size_type j = 0; j < field_index.size(); ++j) {
              printf(", %lu:%g", field_index[j].first, field_index[j].second);
            }
            printf("]");
          } else if (data.format()[i] == kBytes) {
            const byte_array field_bytes = ::std::get< byte_array >(
                data_input[i]);
            printf(", data_input[%lu] = [%lu, ", i, field_bytes.size());
            for (size_type j = 0; j < field_bytes.size(); ++j) {
              printf("%2.2x", field_bytes[j]);
            }
            printf("]");
          }
        }
        printf(", data_label = %lu:%g", data_label.first, data_label.second);
        printf(", data_field = %lu", data_field);
        printf("\n");
      };
  builder.build(callback);

  // Check the frequency table
  for (const typename value_table::value_type &pair : codec[0].frequency()) {
    for (const char &byte : pair.first) {
      printf("%2.2x", static_cast< uint8_t >(byte));
    }
    printf(": %g\n", pair.second);
  }
}

TEST(CodecBuilder, buildTest) {
  buildTest< DoubleHuffmanCodecBuilder >();
  buildTest< FloatHuffmanCodecBuilder >();
}

}
}  // namespace bytesteady
