/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_CODEC_CODER_HPP_
#define BYTESTEADY_CODEC_CODER_HPP_

#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "bytesteady/data.hpp"
#include "bytesteady/bytehuffman_codec.hpp"
#include "bytesteady/bytepair_codec.hpp"
#include "bytesteady/digram_codec.hpp"
#include "bytesteady/huffman_codec.hpp"
#include "bytesteady/subsample_codec.hpp"

namespace bytesteady {

template < typename D = Data<>, typename C = HuffmanCodec >
class CodecCoder {
 public:
  typedef D data_type;
  typedef C codec_type;
  typedef typename D::field_array field_array;
  typedef typename D::field_variant field_variant;
  typedef typename D::index_array index_array;
  typedef typename D::index_pair index_pair;
  typedef typename C::byte_array byte_array;
  typedef typename C::data_callback data_callback;
  typedef typename C::size_type size_type;
  typedef typename C::size_array size_array;
  typedef ::std::unordered_map< size_type, C > codec_table;

  // Node type for priority queue
  struct Node {
    size_type data_index;
    field_array data_output;
    index_pair data_label;

    bool operator==(const Node &b) const;
    bool operator!=(const Node &b) const;
    bool operator<(const Node &b) const;
    bool operator>(const Node &b) const;
    bool operator<=(const Node &b) const;
    bool operator>=(const Node &b) const;
  };
  typedef Node node_type;
  // This is a min heap for Node
  typedef ::std::priority_queue<
    Node, ::std::vector< Node >, ::std::greater< Node > > node_queue;

  struct Local {
    size_type data_index;
    field_array data_input;
    index_pair data_label;
    field_array data_output;
  };
  typedef Local local_type;
  typedef ::std::function< void (const Local &) > callback_type;

  CodecCoder(D *d, codec_table *c, const ::std::string &fn, size_type t = 1);
  ~CodecCoder();

  /*
   * Guarantee: when callback() is called, no mutex will be held by the thread.
   */
  void encode(const callback_type &callback);
  void decode(const callback_type &callback);

  void encodeJob(
      const callback_type &callback, ::std::mutex *mutex,
      typename ::std::vector< bool >::reference result);
  void decodeJob(
      const callback_type &callback, ::std::mutex *mutex,
      typename ::std::vector< bool >::reference result);
  /*
   * Returns false if file cannot be written
   */
  bool join();
  void lock();
  void unlock();

  // Write and increase count_
  bool write(const field_array &data_output, const index_pair &data_label,
             size_type data_index);
  void close();

  size_type count();

 private:
  // Foreign object pointers
  D *data_;
  codec_table *codec_;

  // Member variables
  ::std::string file_;
  size_type thread_size_;
  FILE *fp_ = nullptr;

  // Thread, mutex and result container
  ::std::vector< ::std::thread > threads_;
  ::std::vector< ::std::mutex > mutexes_;
  ::std::vector< bool > results_;

  // Mutex to update count and write to file
  ::std::mutex write_mutex_;
  node_queue queue_;
  size_type count_ = 0;
};

typedef CodecCoder< DoubleData, BytehuffmanCodec > DoubleBytehuffmanCodecCoder;
typedef CodecCoder< FloatData, BytehuffmanCodec > FloatBytehuffmanCodecCoder;
typedef CodecCoder< DoubleData, BytepairCodec > DoubleBytepairCodecCoder;
typedef CodecCoder< FloatData, BytepairCodec > FloatBytepairCodecCoder;
typedef CodecCoder< DoubleData, DigramCodec > DoubleDigramCodecCoder;
typedef CodecCoder< FloatData, DigramCodec > FloatDigramCodecCoder;
typedef CodecCoder< DoubleData, HuffmanCodec > DoubleHuffmanCodecCoder;
typedef CodecCoder< FloatData, HuffmanCodec > FloatHuffmanCodecCoder;
typedef CodecCoder< DoubleData, SubsampleCodec > DoubleSubsampleCodecCoder;
typedef CodecCoder< FloatData, SubsampleCodec > FloatSubsampleCodecCoder;

}  // namespace bytesteady

namespace bytesteady {
// Pre-compiled template class instantiatio
extern template class CodecCoder< DoubleData, BytehuffmanCodec >;
extern template class CodecCoder< FloatData, BytehuffmanCodec >;
extern template class CodecCoder< DoubleData, BytepairCodec >;
extern template class CodecCoder< FloatData, BytepairCodec >;
extern template class CodecCoder< DoubleData, DigramCodec >;
extern template class CodecCoder< FloatData, DigramCodec >;
extern template class CodecCoder< DoubleData, HuffmanCodec >;
extern template class CodecCoder< FloatData, HuffmanCodec >;
extern template class CodecCoder< DoubleData, SubsampleCodec >;
extern template class CodecCoder< FloatData, SubsampleCodec >;
}  // namespace bytesteady

#endif  // BYTESTEADY_CODEC_CODER_HPP_
