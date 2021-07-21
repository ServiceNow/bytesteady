/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/nll_loss.hpp"

#include <cmath>

namespace bytesteady {

template < typename T >
typename T::value_type NLLLoss< T >::forward(const T &input, size_type target) {
  // Using max improves stability of computating log of sum of exp.
  value_type max_input = input.max();
  output_ = scratch_.resizeAs(input).copy(input).sub(max_input).exp().sum();
  output_ = max_input + ::std::log(output_) - input(target);
  return output_;
}

template < typename T >
const T &NLLLoss< T >::backward(const T &input, size_type target) {
  // log of sum of exp is output_ + input(target)
  grad_input_.resizeAs(input).copy(input).add(-output_ - input(target)).exp();
  grad_input_[target].add(-1.0);
  return grad_input_;
}

template < typename T >
typename T::value_type NLLLoss< T >::output() const {
  return output_;
}

template < typename T >
const T &NLLLoss< T >::grad_input() const {
  return grad_input_;
}

}  // namespace bytesteady
