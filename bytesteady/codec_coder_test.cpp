/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/codec_coder.hpp"

#include <string>
#include <variant>

#include "bytesteady/codec_builder.hpp"
#include "bytesteady/field_format.hpp"
#include "bytesteady/integer.hpp"
#include "gtest/gtest.h"

namespace bytesteady {
namespace {

template < typename B, typename C >
void encodeDecodeTest() {
  typedef typename B::codec_table codec_table;
  typedef typename B::data_type data_type;
  typedef typename B::gram_array gram_array;
  typedef typename B::local_type builder_local;
  typedef typename B::callback_type builder_callback;
  typedef typename B::size_type size_type;
  typedef typename C::local_type coder_local;
  typedef typename C::callback_type coder_callback;
  typedef typename data_type::byte_array byte_array;
  typedef typename data_type::field_array field_array;
  typedef typename data_type::format_array format_array;
  typedef typename data_type::index_array index_array;
  typedef typename data_type::index_pair index_pair;

  // Build the codec
  ::std::string file = "bytesteady/unittest_train.txt";
  format_array format = {kBytes, kIndex};
  data_type data(file, format);
  gram_array gram = {{1,2,4},{}};
  codec_table codec;
  B builder(&data, &codec, gram);
  builder_callback build_callback = [&](const builder_local &local) -> void {};
  builder.build(build_callback);

  coder_callback encode_decode_callback =
      [&](const coder_local &local) -> void {
        const field_array &data_input = local.data_input;
        const index_pair &data_label = local.data_label;
        const field_array &data_output = local.data_output;
        printf("Encode/Decode count = N/A");
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
        for (size_type i = 0; i < data_output.size(); ++i) {
          if (data.format()[i] == kIndex) {
            const index_array field_index = ::std::get< index_array >(
                data_output[i]);
            printf(", data_output[%lu] = [%lu", i, field_index.size());
            for (size_type j = 0; j < field_index.size(); ++j) {
              printf(", %lu:%g", field_index[j].first, field_index[j].second);
            }
            printf("]");
          } else if (data.format()[i] == kBytes) {
            const byte_array field_bytes = ::std::get< byte_array >(
                data_output[i]);
            printf(", data_output[%lu] = [%lu, ", i, field_bytes.size());
            for (size_type j = 0; j < field_bytes.size(); ++j) {
              printf("%2.2x", field_bytes[j]);
            }
            printf("]");
          }
        }
        printf("\n");
      };

  ::std::string encode_file = "/tmp/unittest_encode.txt";
  size_type encode_thread_size = 2;
  C encode_coder(&data, &codec, encode_file, encode_thread_size);
  data.rewind();
  encode_coder.encode(encode_decode_callback);
  EXPECT_TRUE(encode_coder.join());
  encode_coder.close();

  data_type encoded_data(encode_file, format);
  ::std::string decode_file = "/tmp/unittest_decode.txt";
  size_type decode_thread_size = 3;
  C decode_coder(&encoded_data, &codec, decode_file, decode_thread_size);
  encoded_data.rewind();
  decode_coder.decode(encode_decode_callback);
  EXPECT_TRUE(decode_coder.join());
  decode_coder.close();

  data_type decoded_data(decode_file, format);
  field_array data_input, decoded_input;
  index_pair data_label, decoded_label;
  data.rewind();
  decoded_data.rewind();
  const byte_array *data_field_bytes, *decoded_field_bytes;
  const index_array *data_field_index, *decoded_field_index;
  while (data.getSample(&data_input, &data_label)) {
    ASSERT_TRUE(decoded_data.getSample(&decoded_input, &decoded_label));
    ASSERT_EQ(data_input.size(), decoded_input.size());
    for (size_type i = 0; i < data_input.size(); ++i) {
      if ((data_field_index = ::std::get_if< index_array >(&data_input[i]))
          != nullptr) {
        decoded_field_index = ::std::get_if< index_array >(&decoded_input[i]);
        ASSERT_NE(nullptr, decoded_field_index);
        ASSERT_EQ(data_field_index->size(), decoded_field_index->size());
        for (size_type j = 0; j < data_field_index->size(); ++j) {
          const index_pair &data_pair = (*data_field_index)[j];
          const index_pair &decoded_pair = (*decoded_field_index)[j];
          EXPECT_EQ(data_pair.first, decoded_pair.first);
          EXPECT_FLOAT_EQ(data_pair.second, decoded_pair.second);
        }
      } else if (
          (data_field_bytes = ::std::get_if< byte_array >(&data_input[i]))
          != nullptr) {
        decoded_field_bytes = ::std::get_if< byte_array >(&decoded_input[i]);
        ASSERT_NE(nullptr, decoded_field_bytes);
        ASSERT_EQ(data_field_bytes->size(), decoded_field_bytes->size());
        for (size_type j = 0; j < data_field_bytes->size(); ++j) {
          EXPECT_EQ((*data_field_bytes)[j], (*decoded_field_bytes)[j]);
        }
      }
    }
  }
}

TEST(CodeCoderTest, encodeDecodeTest) {
  encodeDecodeTest< DoubleHuffmanCodecBuilder, DoubleHuffmanCodecCoder >();
  encodeDecodeTest< FloatHuffmanCodecBuilder, FloatHuffmanCodecCoder >();
}

}  // namespace
}  // namespace bytesteady
