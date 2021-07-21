/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/hinge_loss.hpp"

#include "thunder/tensor.hpp"

#include "bytesteady/hinge_loss-inl.hpp"

namespace bytesteady {

// Pre-compiled template class instantiation
template class HingeLoss< ::thunder::DoubleTensor >;
template class HingeLoss< ::thunder::FloatTensor >;

}  // namespace bytesteady
