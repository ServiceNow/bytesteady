/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/train.hpp"

#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <thread>

#include "gtest/gtest.h"

namespace bytesteady {
namespace {

template < typename T >
void trainTest() {
  typedef T train_type;
  typedef typename T::callback_type callback_type;
  typedef typename T::data_type data_type;
  typedef typename T::loss_type loss_type;
  typedef typename T::model_type model_type;
  typedef typename T::local_type local_type;
  typedef typename T::universum_type universum_type;
  typedef typename data_type::byte_array byte_array;
  typedef typename data_type::field_array field_array;
  typedef typename data_type::format_array format_array;
  typedef typename data_type::index_array index_array;
  typedef typename data_type::index_pair index_pair;
  typedef typename model_type::gram_array gram_array;
  typedef typename model_type::size_storage size_storage;
  typedef typename model_type::size_type size_type;
  typedef typename model_type::tensor_array tensor_array;
  typedef typename model_type::tensor_type tensor_type;
  typedef typename model_type::value_type value_type;

  // Create data
  ::std::string data_file = "bytesteady/unittest_train.txt";
  format_array data_format = {kBytes, kIndex};
  data_type data(data_file, data_format);

  // Create universum
  universum_type universum;

  // Create model
  size_storage model_input_size = {1000000, 16};
  size_type model_output_size = 4;
  size_type model_dimension = 10;
  gram_array model_gram = {{1,2,4,8},{}};
  uint64_t model_seed = 1946;
  value_type model_mu = 0.0;
  value_type model_sigma = 1.0;
  model_type model(model_input_size, model_output_size, model_dimension,
                   model_gram, model_seed);
  model.initialize(model_mu, model_sigma);

  // Create loss
  loss_type loss;

  // Create train object
  value_type train_a = 0.01;
  value_type train_b = 0.0;
  value_type train_alpha = 0.1;
  value_type train_lambda = 0.00001;
  size_type train_n = 10;
  value_type train_rho = 0.1;
  size_type train_thread_size = 3;
  T train(&data, &universum, &model, &loss, train_a, train_b, train_alpha,
          train_lambda, train_n, train_rho, train_thread_size);

  // Create a callback
  ::std::mutex callback_mutex;
  callback_type callback =
      [&](const local_type &local) -> void
      {
        ::std::lock_guard< ::std::mutex > lock(callback_mutex);
        using ::std::printf;
        // Print thread
        ::std::string thread_id = dynamic_cast< ::std::ostringstream &>(
             ::std::ostringstream() << ::std::hex <<
             ::std::this_thread::get_id()).str();
        printf("Train thread = %s", thread_id.c_str());
        // Print train
        printf(", step = %lu, rate = %.8g, data_objective = %.8g"
               ", universum_objective = %.8g", train.step(), train.rate(),
               local.data_objective, local.universum_objective);
        // Print data and universum
        const field_array &data_input = local.data_input;
        const index_pair &data_label = local.data_label;
        const field_array &universum_input = local.universum_input;
        const index_pair &universum_label = local.universum_label;
        for (size_type i = 0; i < data_input.size(); ++i) {
          if (data.format()[i] == kIndex) {
            const index_array field_index = ::std::get< index_array >(
                data_input[i]);
            printf(", data_input[%lu] = [%lu", i, field_index.size());
            for (size_type j = 0; j < field_index.size(); ++j) {
              printf(", %lu:%g", field_index[j].first, field_index[j].second);
            }
            printf("]");
          } else if (data.format()[i] == kBytes) {
            const byte_array field_bytes = ::std::get< byte_array >(
                data_input[i]);
            printf(", data_input[%lu] = [%lu, ", i, field_bytes.size());
            for (size_type j = 0; j < field_bytes.size(); ++j) {
              printf("%2.2x", field_bytes[j]);
            }
            printf("]");
          }
        }
        printf(", data_label = %lu:%g", data_label.first, data_label.second);
        for (size_type i = 0; i < universum_input.size(); ++i) {
          if (universum_input.size() == universum_input.size()) {
            if (data.format()[i] == kIndex) {
              const index_array field_index = ::std::get< index_array >(
                  universum_input[i]);
              printf(", universum_input[%lu] = [%lu", i, field_index.size());
              for (size_type j = 0; j < field_index.size(); ++j) {
                printf(", %lu:%g", field_index[j].first, field_index[j].second);
              }
              printf("]");
            } else if (data.format()[i] == kBytes) {
              const byte_array field_bytes = ::std::get< byte_array >(
                  universum_input[i]);
              printf(", universum_input[%lu] = [%lu, ", i, field_bytes.size());
              for (size_type j = 0; j < field_bytes.size(); ++j) {
                printf("%2.2x", field_bytes[j]);
              }
              printf("]");
            }
          }
        }
        printf(", universum_label = %lu:%g",
               universum_label.first, universum_label.second);
        // Print model and loss
        const model_type &local_model = local.model;
        const loss_type &local_loss = local.loss;
        const tensor_array &input_embedding = local_model.input_embedding();
        const tensor_type &output_embedding = local_model.output_embedding();
        const tensor_type &output = local_model.output();
        const tensor_type &grad_output = local_loss.grad_input();
        const tensor_type &feature = local_model.feature();
        const tensor_type &grad_feature = local_model.grad_feature();
        for (size_type i = 0; i < input_embedding.size(); ++i) {
          printf(", input_embedding[%lu] = [%lu, %lu, %.8g, %.8g, %.8g, %.8g]",
                 i, input_embedding[i].size(0), input_embedding[i].size(1),
                 input_embedding[i].mean(), input_embedding[i].std(),
                 input_embedding[i].min(), input_embedding[i].max());
        }
        printf(", output_embedding = [%lu, %lu, %.8g, %.8g, %.8g, %8g]",
               output_embedding.size(0), output_embedding.size(1),
               output_embedding.mean(), output_embedding.std(),
               output_embedding.min(), output_embedding.max());
        printf(", output = [%lu, %.8g, %.8g, %.8g, %.8g]", output.size(0),
               output.mean(), output.std(), output.min(), output.max());
        printf(", grad_output = [%lu, %.8g, %.8g, %.8g, %.8g]",
               grad_output.size(0), grad_output.mean(), grad_output.std(),
               grad_output.min(), grad_output.max());
        printf(", feature = [%lu, %.8g, %.8g, %.8g, %.8g]", feature.size(0),
               feature.mean(), feature.std(), feature.min(), feature.max());
        printf(", grad_feature = [%lu, %.8g, %.8g, %.8g, %.8g]",
               grad_feature.size(0), grad_feature.mean(), grad_feature.std(),
               grad_feature.min(), grad_feature.max());
        printf("\n");
      };

  // Start the training job
  size_type epoches = 4;
  for (size_type i = 0; i < epoches; ++i) {
    data.rewind();
    train.train(callback);
    train.lock();
    printf("Locked train\n");
    train.unlock();
    printf("Unlocked train\n");
    train.join();
    EXPECT_EQ(20, data.count());
    EXPECT_EQ(20 * train_n * (i + 1), universum.count());
    EXPECT_EQ(20 * (i + 1), train.step());
  }
}

TEST(TrainTest, trainTest) {
  trainTest< DoubleFNVNLLTrain >();
}

}  // namespace
}  // namespace bytesteady
