/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_FLAGS_HPP_
#define BYTESTEADY_FLAGS_HPP_

#include "gflags/gflags.h"

DECLARE_string(data_file);
DECLARE_string(data_format);

DECLARE_string(model_input_size);
DECLARE_uint64(model_output_size);
DECLARE_uint64(model_dimension);
DECLARE_string(model_gram);
DECLARE_uint64(model_seed);
DECLARE_double(model_mu);
DECLARE_double(model_sigma);

DECLARE_double(train_a);
DECLARE_double(train_b);
DECLARE_double(train_alpha);
DECLARE_double(train_lambda);
DECLARE_uint64(train_n);
DECLARE_double(train_rho);
DECLARE_uint64(train_thread_size);

DECLARE_uint64(test_label_size);
DECLARE_uint64(test_thread_size);

DECLARE_string(infer_file);
DECLARE_uint64(infer_label_size);

DECLARE_uint64(driver_epoch_size);
DECLARE_string(driver_location);
DECLARE_uint64(driver_save);
DECLARE_bool(driver_resume);
DECLARE_bool(driver_debug);
DECLARE_double(driver_log_interval);
DECLARE_int64(driver_log_precision);
DECLARE_double(driver_checkpoint_interval);
DECLARE_string(driver_model);

DECLARE_string(joe_task);
DECLARE_string(joe_tensor);
DECLARE_string(joe_hash);
DECLARE_string(joe_loss);

#endif  // BYTESTEADY_FLAGS_HPP_
