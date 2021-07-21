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

#include "bytesteady/flags.hpp"

#include "gflags/gflags.h"

DEFINE_string(data_file, "bytesteady/unittest_train.txt",
              "data input file name");
DEFINE_string(data_format, "kBytes,kIndex", "a comma-separated list of kBytes"
              " or kIndex representing field types");

DEFINE_string(model_input_size, "16,16", "a comma-seperated list of numbers"
              " representing input embedding size");
DEFINE_uint64(model_output_size, 4, "output embedding size");
DEFINE_uint64(model_dimension, 4, "embedding dimension");
DEFINE_string(model_gram, "{1,2,4,8},{}", "list of grams for each field");
DEFINE_uint64(model_seed, 1946, "hashing seed");
DEFINE_double(model_mu, 0.0, "mean of the Gaussian distribution used to"
              " initialize parameters");
DEFINE_double(model_sigma, 1.0, "standard deviation of the Gaussian"
               " distribution used to initialize parameters");

DEFINE_double(train_a, 0.1, "initial learning rate");
DEFINE_double(train_b, 0.0, "eventual learning rate");
DEFINE_double(train_alpha, 0.0, "learning rate change factor");
DEFINE_double(train_lambda, 0.0, "weight decay");
DEFINE_uint64(train_n, 0, "number of universum samples for every data sample");
DEFINE_double(train_rho, 1.0, "universum learning rate factor");
DEFINE_uint64(train_thread_size, 4, "number of threads for training");

DEFINE_uint64(test_label_size, 3, "size of label to consider during"
              " testing");
DEFINE_uint64(test_thread_size, 1, "number of threads for testing");

DEFINE_string(infer_file, "bytesteady/unittest_result.txt",
              "inference result file");
DEFINE_uint64(infer_label_size, 3, "size of label to consider during"
              " inference");

DEFINE_uint64(driver_epoch_size, 1, "number of epoches for training");
DEFINE_string(driver_location, "", "location to store model checkpoint");
DEFINE_uint64(driver_save, 0, "epoch interval to save the model, 0 to disable");
DEFINE_bool(driver_resume, false, "whether to resume training");
DEFINE_bool(driver_debug, false,
            "whether to log in verbose mode for debugging");
DEFINE_double(driver_log_interval, 5.0, "time interval for logging");
DEFINE_int64(driver_log_precision, 4, "numerical precision for logging");
DEFINE_double(driver_checkpoint_interval, 3600.0,
              "time interval for checkpointing");
DEFINE_string(
    driver_model, "model.tdb",
    "testing or inference model file relative to checkpoint location");

DEFINE_string(joe_task, "train", "task to run, can be train, test or infer");
DEFINE_string(joe_tensor, "double", "type of tensor, can be double or float");
DEFINE_string(joe_hash, "fnv", "type of hash, can be fnv or city");
DEFINE_string(joe_loss, "nll", "type of loss, can be nll or hinge");
