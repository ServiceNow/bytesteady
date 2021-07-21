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

#include "gtest/gtest.h"
#include "thunder/random.hpp"

namespace bytesteady {
namespace {

template < typename L >
void forwardBackwardTest() {
  typedef typename L::tensor_type tensor_type;
  typedef typename L::size_type size_type;
  typedef typename L::value_type value_type;

  ::thunder::Random< tensor_type > random;

  // Create the loss
  L loss;

  // Create input and target
  tensor_type input(7);
  random.normal(input, 0.0, 1.0);
  size_type target = 4;

  // Forward propagation
  value_type output = loss.forward(input, target);
  value_type output_reference = -input(target) + ::std::log(
      input.clone().exp().sum());
  EXPECT_FLOAT_EQ(output_reference, output);

  // Backward propagation
  tensor_type grad_input = loss.backward(input, target);
  EXPECT_EQ(1, grad_input.dimension());
  EXPECT_EQ(input.size(0), grad_input.size(0));
  tensor_type grad_input_reference(input.clone());
  grad_input_reference.exp();
  grad_input_reference.div(grad_input_reference.sum());
  grad_input_reference[target].add(-1);
  for (int i = 0; i < grad_input_reference.size(0); ++i) {
    EXPECT_FLOAT_EQ(grad_input_reference(i), grad_input(i));
  }
}

TEST(NLLLossTest, forwardBackwardTest) {
  forwardBackwardTest< DoubleNLLLoss >();
  forwardBackwardTest< FloatNLLLoss >();
}

}
}  // namespace bytesteady
