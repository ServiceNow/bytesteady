/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/universum.hpp"

#include "thunder/tensor.hpp"

#include "bytesteady/universum-inl.hpp"

namespace bytesteady {
// Template class instantiation
template class Universum< ::thunder::DoubleTensor >;
template class Universum< ::thunder::FloatTensor >;
}  // namespace bytesteady
