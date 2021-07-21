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
