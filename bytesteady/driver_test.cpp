/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/driver.hpp"

#include "bytesteady/flags.hpp"
#include "gtest/gtest.h"

namespace bytesteady {
namespace {

void setFlags() {
  // Data configuration
  FLAGS_data_file = "bytesteady/unittest_train.txt";
  FLAGS_data_format = "kBytes,kIndex";

  // Model configuration
  FLAGS_model_input_size = "1024,16";
  FLAGS_model_output_size = 4;
  FLAGS_model_dimension = 16;
  FLAGS_model_gram = "{1,2,4,8},{1:3},{2:2:6}";
  FLAGS_model_seed = 1946;
  FLAGS_model_mu = 0.0;
  FLAGS_model_sigma = 1.0;

  // Train configuration
  FLAGS_train_a = 0.01;
  FLAGS_train_b = 0.0;
  FLAGS_train_alpha = 0.00001;
  FLAGS_train_lambda = 0.00001;
  FLAGS_train_n = 4;
  FLAGS_train_rho = 0.25;
  FLAGS_train_thread_size = 4;

  // Test configuration
  FLAGS_test_thread_size = 2;
  FLAGS_test_label_size = 3;

  // Infer configuration
  FLAGS_infer_file = "/tmp/infer.txt";
  FLAGS_infer_label_size = 3;

  // Driver configuration
  FLAGS_driver_epoch_size = 4;
  FLAGS_driver_location = "/tmp";
  FLAGS_driver_save = 2;
  FLAGS_driver_resume = false;
  FLAGS_driver_debug = true;
  FLAGS_driver_log_interval = 0.0;
  FLAGS_driver_log_precision = 4;
  FLAGS_driver_checkpoint_interval = 5.0;
  FLAGS_driver_model = "model.tdb";
}

template < typename D >
void trainTest() {
  typedef typename D::data_type data_type;
  typedef typename D::universum_type universum_type;
  typedef typename D::model_type model_type;
  typedef typename D::loss_type loss_type;
  typedef typename D::train_type train_type;
  typedef typename D::test_type test_type;
  typedef typename D::infer_type infer_type;
  typedef typename D::gram_array gram_array;

  // Create the driver object
  setFlags();
  D driver;

  // Check data properties
  const data_type &data = driver.data();
  EXPECT_EQ(FLAGS_data_file, data.file());
  EXPECT_EQ(2, data.format().size());
  EXPECT_EQ(kBytes, data.format()[0]);
  EXPECT_EQ(kIndex, data.format()[1]);

  const universum_type &universum = driver.universum();

  // Check model properties
  const model_type &model = driver.model();
  EXPECT_EQ(2, model.input_size());
  EXPECT_EQ(1024, model.input_embedding_size(0));
  EXPECT_EQ(16, model.input_embedding_size(1));
  EXPECT_EQ(FLAGS_model_output_size, model.output_embedding_size());
  EXPECT_EQ(FLAGS_model_dimension, model.dimension());
  EXPECT_EQ(gram_array({{1,2,4,8},{1,2,3},{2,4,6}}), model.gram());
  EXPECT_EQ(FLAGS_model_seed, model.seed());

  const loss_type &loss = driver.loss();

  // Check train properties
  const train_type &train = driver.train();
  EXPECT_FLOAT_EQ(FLAGS_train_a, train.a());
  EXPECT_FLOAT_EQ(FLAGS_train_b, train.b());
  EXPECT_FLOAT_EQ(FLAGS_train_alpha, train.alpha());
  EXPECT_FLOAT_EQ(FLAGS_train_lambda, train.lambda());
  EXPECT_EQ(FLAGS_train_n, train.n());
  EXPECT_FLOAT_EQ(FLAGS_train_rho, train.rho());
  EXPECT_EQ(FLAGS_train_thread_size, train.thread_size());

  const test_type &test = driver.test();
  EXPECT_EQ(FLAGS_test_label_size, test.label_size());

  const infer_type &infer = driver.infer();
  EXPECT_EQ(FLAGS_infer_label_size, infer.label_size());

  driver.runTrain();
}

TEST(DriverTest, trainTest) {
  trainTest< DoubleFNVNLLDriver >();
}

template < typename D >
void testTest() {
  typedef typename D::data_type data_type;
  typedef typename D::universum_type universum_type;
  typedef typename D::model_type model_type;
  typedef typename D::loss_type loss_type;
  typedef typename D::train_type train_type;
  typedef typename D::test_type test_type;
  typedef typename D::infer_type infer_type;
  typedef typename D::gram_array gram_array;

  // Create the driver object
  setFlags();
  D driver;

  // Check data properties
  const data_type &data = driver.data();
  EXPECT_EQ(FLAGS_data_file, data.file());
  EXPECT_EQ(2, data.format().size());
  EXPECT_EQ(kBytes, data.format()[0]);
  EXPECT_EQ(kIndex, data.format()[1]);

  const universum_type &universum = driver.universum();

  // Check model properties
  const model_type &model = driver.model();
  EXPECT_EQ(2, model.input_size());
  EXPECT_EQ(1024, model.input_embedding_size(0));
  EXPECT_EQ(16, model.input_embedding_size(1));
  EXPECT_EQ(FLAGS_model_output_size, model.output_embedding_size());
  EXPECT_EQ(FLAGS_model_dimension, model.dimension());
  EXPECT_EQ(gram_array({{1,2,4,8},{1,2,3},{2,4,6}}), model.gram());
  EXPECT_EQ(FLAGS_model_seed, model.seed());

  const loss_type &loss = driver.loss();

  // Check train properties
  const train_type &train = driver.train();
  EXPECT_FLOAT_EQ(FLAGS_train_a, train.a());
  EXPECT_FLOAT_EQ(FLAGS_train_b, train.b());
  EXPECT_FLOAT_EQ(FLAGS_train_alpha, train.alpha());
  EXPECT_FLOAT_EQ(FLAGS_train_lambda, train.lambda());
  EXPECT_EQ(FLAGS_train_n, train.n());
  EXPECT_FLOAT_EQ(FLAGS_train_rho, train.rho());
  EXPECT_EQ(FLAGS_train_thread_size, train.thread_size());

  const test_type &test = driver.test();
  EXPECT_EQ(FLAGS_test_label_size, test.label_size());

  const infer_type &infer = driver.infer();
  EXPECT_EQ(FLAGS_infer_label_size, infer.label_size());

  driver.runTest();
}

TEST(DriverTest, testTest) {
  testTest< DoubleFNVNLLDriver >();
}

template < typename D >
void inferTest() {
  typedef typename D::data_type data_type;
  typedef typename D::universum_type universum_type;
  typedef typename D::model_type model_type;
  typedef typename D::loss_type loss_type;
  typedef typename D::train_type train_type;
  typedef typename D::test_type test_type;
  typedef typename D::infer_type infer_type;
  typedef typename D::gram_array gram_array;

  setFlags();
  FLAGS_data_file = "bytesteady/unittest_infer.txt";
  D driver;

  const data_type &data = driver.data();
  EXPECT_EQ(FLAGS_data_file, data.file());
  EXPECT_EQ(2, data.format().size());
  EXPECT_EQ(kBytes, data.format()[0]);
  EXPECT_EQ(kIndex, data.format()[1]);

  const universum_type &universum = driver.universum();

  const model_type &model = driver.model();
  EXPECT_EQ(2, model.input_size());
  EXPECT_EQ(1024, model.input_embedding_size(0));
  EXPECT_EQ(16, model.input_embedding_size(1));
  EXPECT_EQ(FLAGS_model_output_size, model.output_embedding_size());
  EXPECT_EQ(FLAGS_model_dimension, model.dimension());
  EXPECT_EQ(gram_array({{1,2,4,8},{1,2,3},{2,4,6}}), model.gram());
  EXPECT_EQ(FLAGS_model_seed, model.seed());

  const loss_type &loss = driver.loss();

  const train_type &train = driver.train();
  EXPECT_FLOAT_EQ(FLAGS_train_a, train.a());
  EXPECT_FLOAT_EQ(FLAGS_train_b, train.b());
  EXPECT_FLOAT_EQ(FLAGS_train_alpha, train.alpha());
  EXPECT_FLOAT_EQ(FLAGS_train_lambda, train.lambda());
  EXPECT_EQ(FLAGS_train_n, train.n());
  EXPECT_FLOAT_EQ(FLAGS_train_rho, train.rho());
  EXPECT_EQ(FLAGS_train_thread_size, train.thread_size());

  const test_type &test = driver.test();
  EXPECT_EQ(FLAGS_test_label_size, test.label_size());

  const infer_type &infer = driver.infer();
  EXPECT_EQ(FLAGS_infer_label_size, infer.label_size());

  driver.runInfer();
}

TEST(DriverTest, inferTest) {
  inferTest< DoubleFNVNLLDriver >();
}

}  // namespace
}  // namespace bytesteady
