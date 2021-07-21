/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/data.hpp"

#include "thunder/tensor.hpp"

#include "bytesteady/data-inl.hpp"

namespace bytesteady {
// Template class instantiation
template class Data< ::thunder::DoubleTensor >;
template class Data< ::thunder::FloatTensor >;
}  // namespace bytesteady
