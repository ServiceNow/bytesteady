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

#ifndef BYTESTEADY_NLL_LOSS_HPP_
#define BYTESTEADY_NLL_LOSS_HPP_

#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename T = ::thunder::DoubleTensor >
class NLLLoss {
 public:
  typedef T tensor_type;
  typedef typename T::value_type value_type;
  typedef typename T::size_type size_type;

  value_type forward(const T &input, size_type target);
  // Backward ssumes that the preceding forward was called.
  const T &backward(const T &input, size_type target);

  value_type output() const;
  const T &grad_input() const;

 private:
  value_type output_;
  T grad_input_;
  T scratch_;
};

typedef NLLLoss< ::thunder::DoubleTensor > DoubleNLLLoss;
typedef NLLLoss< ::thunder::FloatTensor > FloatNLLLoss;
// Already exists: typedef DoubleNLLLoss NLLLoss;

}  // namespace bytesteady

namespace bytesteady {

// Pre-compiled template class instantiation
extern template class NLLLoss< ::thunder::DoubleTensor >;
extern template class NLLLoss< ::thunder::FloatTensor >;

}  // namespace bytesteady

#endif  // BYTESTEADY_NLL_LOSS_HPP_
