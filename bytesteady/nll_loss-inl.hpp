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
