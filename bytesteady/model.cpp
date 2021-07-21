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

#include "bytesteady/model.hpp"

#include "bytesteady/hash.hpp"
#include "thunder/serializer.hpp"
#include "thunder/serializer/binary_protocol.hpp"
#include "thunder/serializer/serializer.hpp"
#include "thunder/serializer/static.hpp"
#include "thunder/serializer/text_protocol.hpp"
#include "thunder/tensor.hpp"

#include "bytesteady/model-inl.hpp"

#include "thunder/serializer/binary_protocol-inl.hpp"
#include "thunder/serializer/serializer-inl.hpp"
#include "thunder/serializer/static-inl.hpp"
#include "thunder/serializer/text_protocol-inl.hpp"

namespace bytesteady {

// Template class instantiation
template class Model< ::thunder::DoubleTensor, FNVHash >;
template class Model< ::thunder::FloatTensor, FNVHash >;
template class Model< ::thunder::DoubleTensor, CityHash >;
template class Model< ::thunder::FloatTensor, CityHash >;

}  // namespace bytesteady

namespace thunder {
namespace serializer {

// Template serializer instantiation
#define BYTESTEADY_MODEL_INSTANTIATE_SERIALIZE(T, H)    \
  template void save(                                   \
      StringBinarySerializer *s,                        \
      const ::bytesteady::Model< T, H > &m);            \
  template void StringBinarySerializer::save(           \
      const ::bytesteady::Model< T, H > &m);            \
  template void load(                                   \
      StringBinarySerializer *s,                        \
      ::bytesteady::Model< T, H > *m);                  \
  template void StringBinarySerializer::load(           \
      ::bytesteady::Model< T, H > *m);                  \
  template void save(                                   \
      FileBinarySerializer *s,                          \
      const ::bytesteady::Model< T, H > &m);            \
  template void FileBinarySerializer::save(             \
      const ::bytesteady::Model< T, H > &m);            \
  template void load(                                   \
      FileBinarySerializer *s,                          \
      ::bytesteady::Model< T, H > *m);                  \
  template void FileBinarySerializer::load(             \
      ::bytesteady::Model< T, H > *m);                  \
  template void save(                                   \
      StringTextSerializer *s,                          \
      const ::bytesteady::Model< T, H > &m);            \
  template void StringTextSerializer::save(             \
      const ::bytesteady::Model< T, H > &m);            \
  template void load(                                   \
      StringTextSerializer *s,                          \
      ::bytesteady::Model< T, H > *m);                  \
  template void StringTextSerializer::load(             \
      ::bytesteady::Model< T, H > *m);                  \
  template void save(                                   \
      FileTextSerializer *s,                            \
      const ::bytesteady::Model< T, H > &m);            \
  template void FileTextSerializer::save(               \
      const ::bytesteady::Model< T, H > &m);            \
  template void load(                                   \
      FileTextSerializer *s,                            \
      ::bytesteady::Model< T, H > *m);                  \
  template void FileTextSerializer::load(               \
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
