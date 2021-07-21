/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/hinge_loss.hpp"

#include <cmath>

namespace bytesteady {

template < typename T >
typename T::value_type HingeLoss< T >::forward(
    const T &input, size_type target) {
  scratch_.resizeAs(input).copy(input).sub(input(target)).add(1.0).fmax(0.0);
  scratch_[target].zero();
  output_ = scratch_.sum();
  return output_;
}

template < typename T >
const T &HingeLoss< T >::backward(const T &input, size_type target) {
  grad_input_.resizeAs(input).copy(scratch_).isgreater(0.0);
  grad_input_[target].fill(-grad_input_.sum());
  return grad_input_;
}

template < typename T >
typename T::value_type HingeLoss< T >::output() const {
  return output_;
}

template < typename T >
const T &HingeLoss< T >::grad_input() const {
  return grad_input_;
}

}  // namespace bytesteady
