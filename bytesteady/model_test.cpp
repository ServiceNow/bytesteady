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

#include "bytesteady/model.hpp"

#include "gtest/gtest.h"
#include "thunder/linalg.hpp"
#include "thunder/random.hpp"
#include "thunder/serializer.hpp"

namespace bytesteady {
namespace {

template < typename M >
void forwardUpdateTest() {
  typedef typename M::byte_array byte_array;
  typedef typename M::field_array field_array;
  typedef typename M::index_array index_array;
  typedef typename M::size_type size_type;
  typedef typename M::tensor_type tensor_type;
  typedef typename M::tensor_array tensor_array;
  typedef typename M::value_type value_type;

  ::thunder::Random< tensor_type > random;

  // Create Model
  M model({16, 32}, 3, 10, {{},{1,2,3,4}}, 1946);
  model.initialize(0.0, 1.0);

  // Create input
  field_array input;
  input.push_back(index_array{
      ::std::make_pair(size_type(4), value_type(0.6)),
      ::std::make_pair(size_type(3), value_type(0.88))});
  input.push_back(byte_array({22, 0, 255, 4, 9, 88, 126, 30}));

  // Forward on the model
  const tensor_type &output = model.forward(input);
  EXPECT_EQ(1, output.dimension());
  EXPECT_EQ(3, output.size(0));
  // (TODO): check correctness of output

  // Create gradient with respect to the output
  tensor_type grad_output = random.normal(
      tensor_type(output.size(0)), 0.0, 1.0);

  // Update the parameters
  model.update(input, grad_output, 0.001, 0.00001);
  // TODO(Xiang): check correctness of updated parameters
}

TEST(ModelTest, forwardUpdateTest) {
  forwardUpdateTest< DoubleFNVModel >();
}

template < typename M >
void saveLoadTest() {
  typedef typename M::size_type size_type;
  typedef typename M::tensor_type tensor_type;
  typedef typename M::tensor_array tensor_array;

  ::thunder::StringBinarySerializer serializer;

  // Create Model
  M model1({16, 32}, 3, 10, {{1,2,3,4,5}}, 1948);
  // Save model to file
  serializer.save(model1);
  // Create another model
  M model2({2, 9}, 2, 7, {{3}}, 1999);
  // Load model
  serializer.load(&model2);

  EXPECT_EQ(model1.gram(), model2.gram());
  EXPECT_EQ(model1.seed(), model2.seed());
  EXPECT_EQ(model1.input_size(), model2.input_size());
  EXPECT_EQ(model1.dimension(), model2.dimension());
  for (size_type i = 0; i < model1.input_size(); ++i) {
    EXPECT_EQ(model1.input_embedding_size(i), model2.input_embedding_size(i));
  }
  EXPECT_EQ(model1.output_embedding_size(), model2.output_embedding_size());

  const tensor_array &input_embedding1 = model1.input_embedding();
  const tensor_array &input_embedding2 = model2.input_embedding();
  for (size_type i = 0; i < input_embedding1.size(); ++i) {
    for (size_type j = 0; j < input_embedding1[i].size(0); ++j) {
      for (size_type k = 0; k < input_embedding1[i].size(1); ++k) {
        EXPECT_FLOAT_EQ(input_embedding1[i](j, k), input_embedding2[i](j, k));
      }
    }
  }

  const tensor_type &output_embedding1 = model1.output_embedding();
  const tensor_type &output_embedding2 = model2.output_embedding();
  for (size_type i = 0; i < output_embedding1.size(0); ++i) {
    for (size_type j = 0; j < output_embedding1.size(1); ++j) {
      EXPECT_FLOAT_EQ(output_embedding1(i, j), output_embedding2(i, j));
    }
  }
}

TEST(ModelTest, saveLoadTest) {
  saveLoadTest< DoubleFNVModel >();
}

template < typename M >
void cloneTest() {
  typedef typename M::size_type size_type;
  typedef typename M::tensor_type tensor_type;
  typedef typename M::tensor_array tensor_array;

  // Create Model
  M model1({16, 32}, 3, 10, {{1,2,3,4,5}}, 1948);

  // Clone with sharing
  M model2 = model1.clone();
  EXPECT_EQ(model1.gram(), model2.gram());
  EXPECT_EQ(model1.seed(), model2.seed());
  EXPECT_EQ(model1.input_size(), model2.input_size());
  EXPECT_EQ(model1.dimension(), model2.dimension());
  for (size_type i = 0; i < model1.input_size(); ++i) {
    EXPECT_EQ(model1.input_embedding_size(i), model2.input_embedding_size(i));
  }
  EXPECT_EQ(model1.output_embedding_size(), model2.output_embedding_size());
  const tensor_array &input_embedding1 = model1.input_embedding();
  const tensor_array &input_embedding2 = model2.input_embedding();
  for (size_type i = 0; i < input_embedding1.size(); ++i) {
    EXPECT_EQ(input_embedding1[i].storage(), input_embedding2[i].storage());
    for (size_type j = 0; j < input_embedding1[i].size(0); ++j) {
      for (size_type k = 0; k < input_embedding1[i].size(1); ++k) {
        EXPECT_FLOAT_EQ(input_embedding1[i](j, k), input_embedding2[i](j, k));
      }
    }
  }
  const tensor_type &output_embedding1 = model1.output_embedding();
  const tensor_type &output_embedding2 = model2.output_embedding();
  EXPECT_EQ(output_embedding1.storage(), output_embedding2.storage());
  for (size_type i = 0; i < output_embedding1.size(0); ++i) {
    for (size_type j = 0; j < output_embedding1.size(1); ++j) {
      EXPECT_FLOAT_EQ(output_embedding1(i, j), output_embedding2(i, j));
    }
  }
  EXPECT_NE(model1.feature().storage(), model2.feature().storage());
  EXPECT_NE(model1.grad_feature().storage(), model2.grad_feature().storage());
  EXPECT_NE(model1.output().storage(), model2.output().storage());

  // Clone without sharing
  M model3 = model1.clone(false);
  EXPECT_EQ(model1.gram(), model3.gram());
  EXPECT_EQ(model1.seed(), model3.seed());
  EXPECT_EQ(model1.input_size(), model3.input_size());
  EXPECT_EQ(model1.dimension(), model3.dimension());
  for (size_type i = 0; i < model1.input_size(); ++i) {
    EXPECT_EQ(model1.input_embedding_size(i), model3.input_embedding_size(i));
  }
  EXPECT_EQ(model1.output_embedding_size(), model3.output_embedding_size());
  const tensor_array &input_embedding3 = model3.input_embedding();
  for (size_type i = 0; i < input_embedding1.size(); ++i) {
    EXPECT_NE(input_embedding1[i].storage(), input_embedding3[i].storage());
    for (size_type j = 0; j < input_embedding1[i].size(0); ++j) {
      for (size_type k = 0; k < input_embedding1[i].size(1); ++k) {
        EXPECT_FLOAT_EQ(input_embedding1[i](j, k), input_embedding3[i](j, k));
      }
    }
  }
  const tensor_type &output_embedding3 = model3.output_embedding();
  EXPECT_NE(output_embedding1.storage(), output_embedding3.storage());
  for (size_type i = 0; i < output_embedding1.size(0); ++i) {
    for (size_type j = 0; j < output_embedding1.size(1); ++j) {
      EXPECT_FLOAT_EQ(output_embedding1(i, j), output_embedding3(i, j));
    }
  }
  EXPECT_NE(model1.feature().storage(), model3.feature().storage());
  EXPECT_NE(model1.grad_feature().storage(), model3.grad_feature().storage());
  EXPECT_NE(model1.output().storage(), model3.output().storage());
}

TEST(ModelTest, cloneTest) {
  cloneTest< DoubleFNVModel >();
}

}  // namespace
}  // namespace bytesteady
