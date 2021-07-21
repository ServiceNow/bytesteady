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

#ifndef BYTESTEADY_MODEL_HPP_
#define BYTESTEADY_MODEL_HPP_

#include <variant>
#include <vector>

#include "bytesteady/hash.hpp"
#include "bytesteady/integer.hpp"
#include "thunder/linalg.hpp"
#include "thunder/serializer.hpp"
#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename T = ::thunder::DoubleTensor, typename H = FNVHash >
class Model {
 public:
  typedef T tensor_type;
  typedef H hash_type;
  typedef typename T::size_type size_type;
  typedef typename T::size_storage size_storage;
  typedef typename T::value_type value_type;
  typedef ::std::vector< uint8_t > byte_array;
  typedef ::std::pair< size_type, value_type > index_pair;
  typedef ::std::vector< index_pair > index_array;
  typedef ::std::variant< index_array, byte_array > field_variant;
  typedef ::std::vector< field_variant > field_array;
  typedef ::std::vector< size_type > size_array;
  typedef ::std::vector< size_array > gram_array;
  typedef ::std::vector< T > tensor_array;

  // Sizes of the embedding for each field, number of output
  // classes, and dimension of the embedding
  Model(const size_storage &s, size_type c, size_type d,
        const gram_array &g = {{1,2,3,4}}, uint64_t sd = 1946);
  // Construct from input embedding and output embedding
  Model(const tensor_array &ie, const T &oe, const gram_array &g = {{1,2,3,4}},
        uint64_t sd = 1946);

  // Initialize all embedding with given mean and std
  void initialize(value_type mu = 0.0, value_type sigma = 1.0) const;

  // Clone the model
  Model clone(bool share = true) const;

  // Forward with a list of indices and weights for each embedding
  const T &forward(const field_array &input);
  // Update the parameters using the given indices and weights
  void update(const field_array &input, const T &grad_output,
              value_type rate = 1.0, value_type decay = 0.0);

  size_type input_size() const;
  size_storage input_embedding_size() const;
  size_type input_embedding_size(size_type ind) const;
  size_type output_embedding_size() const;
  size_type dimension() const;

  const tensor_array &input_embedding() const;
  void set_input_embedding(const tensor_array &e);

  const T &output_embedding() const;
  void set_output_embedding(const T &e);

  const gram_array &gram() const;
  void set_gram(const gram_array &g);

  uint64_t seed() const;
  void set_seed(uint64_t sd);

  const T &feature() const;
  void set_feature(const T &f);

  const T &grad_feature() const;
  void set_grad_feature(const T &f);

  const T &output() const;
  void set_output(const T &o);

 private:
  H hash_;
  ::thunder::Linalg< T > linalg_;

  tensor_array input_embedding_;
  T output_embedding_;

  gram_array gram_;
  uint64_t seed_;

  T feature_;
  T grad_feature_;
  T output_;

  T scratch_;
};

// Short-hand model class names
typedef Model< ::thunder::DoubleTensor, FNVHash > DoubleFNVModel;
typedef Model< ::thunder::FloatTensor, FNVHash > FloatFNVModel;
typedef Model< ::thunder::DoubleTensor, CityHash > DoubleCityModel;
typedef Model< ::thunder::FloatTensor, CityHash > FloatCityModel;
// Already exists: typedef DoubleFNVModel Model;

}  // namespace bytesteady

namespace thunder {
namespace serializer {

template < typename S, typename T, typename H >
void save(S *s, const ::bytesteady::Model< T, H > &m);

template < typename S, typename T, typename H >
void load(S *s, bytesteady::Model< T, H > *m);

}  // namespace serializer
}  // namespace thunder

namespace bytesteady {

// Pre-compiled template class instantiation
extern template class Model< ::thunder::DoubleTensor, FNVHash >;
extern template class Model< ::thunder::FloatTensor, FNVHash >;
extern template class Model< ::thunder::DoubleTensor, CityHash >;
extern template class Model< ::thunder::FloatTensor, CityHash >;

}  // namespace bytesteady

namespace thunder {
namespace serializer {

// Pre-compiled template serializer instantiation
#define BYTESTEADY_MODEL_INSTANTIATE_SERIALIZE(T, H)    \
  extern template void save(                            \
      StringBinarySerializer *s,                        \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void StringBinarySerializer::save(    \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void load(                            \
      StringBinarySerializer *s,                        \
      ::bytesteady::Model< T, H > *m);                  \
  extern template void StringBinarySerializer::load(    \
      ::bytesteady::Model< T, H > *m);                  \
  extern template void save(                            \
      FileBinarySerializer *s,                          \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void FileBinarySerializer::save(      \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void load(                            \
      FileBinarySerializer *s,                          \
      ::bytesteady::Model< T, H > *m);                  \
  extern template void FileBinarySerializer::load(      \
      ::bytesteady::Model< T, H > *m);                  \
  extern template void save(                            \
      StringTextSerializer *s,                          \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void StringTextSerializer::save(      \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void load(                            \
      StringTextSerializer *s,                          \
      ::bytesteady::Model< T, H > *m);                  \
  extern template void StringTextSerializer::load(      \
      ::bytesteady::Model< T, H > *m);                  \
  extern template void save(                            \
      FileTextSerializer *s,                            \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void FileTextSerializer::save(        \
      const ::bytesteady::Model< T, H > &m);            \
  extern template void load(                            \
      FileTextSerializer *s,                            \
      ::bytesteady::Model< T, H > *m);                  \
  extern template void FileTextSerializer::load(        \
      ::bytesteady::Model< T, H > *m);

BYTESTEADY_MODEL_INSTANTIATE_SERIALIZE(
    ::thunder::DoubleTensor, ::bytesteady::FNVHash);
BYTESTEADY_MODEL_INSTANTIATE_SERIALIZE(
    ::thunder::FloatTensor, ::bytesteady::FNVHash);
BYTESTEADY_MODEL_INSTANTIATE_SERIALIZE(
    ::thunder::DoubleTensor, ::bytesteady::CityHash);
BYTESTEADY_MODEL_INSTANTIATE_SERIALIZE(
    ::thunder::FloatTensor, ::bytesteady::CityHash);

#undef BYTESTEADY_MODEL_INSTANTIATE_SERIALIZE

}  // namespace serializer
}  // namespace thunder

#endif  // BYTESTEADY_MODEL_HPP_
