/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/nll_loss.hpp"

#include "thunder/tensor.hpp"

#include "bytesteady/nll_loss-inl.hpp"

namespace bytesteady {

// Pre-compiled template class instantiation
template class NLLLoss< ::thunder::DoubleTensor >;
template class NLLLoss< ::thunder::FloatTensor >;

}  // namespace bytesteady
