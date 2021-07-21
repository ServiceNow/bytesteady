/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/driver.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <mutex>
#include <regex>
#include <string>
#include <system_error>
#include <thread>
#include <variant>

#include "bytesteady/field_format.hpp"
#include "bytesteady/flags.hpp"
#include "glog/logging.h"
#include "thunder/serializer.hpp"

namespace bytesteady {

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
Driver< D, U, M, L, T, V, I >::Driver() :
    data_(FLAGS_data_file, parseDataFormat()),
    universum_(),
    model_(parseModelInputSize(), FLAGS_model_output_size,
           FLAGS_model_dimension, parseModelGram(), FLAGS_model_seed),
    loss_(),
    train_(&data_, &universum_, &model_, &loss_,
          static_cast< value_type >(FLAGS_train_a),
          static_cast< value_type >(FLAGS_train_b),
          static_cast< value_type >(FLAGS_train_alpha),
          static_cast< value_type >(FLAGS_train_lambda),
          FLAGS_train_n, static_cast< value_type >(FLAGS_train_rho),
          FLAGS_train_thread_size),
    test_(&data_, &model_, &loss_, FLAGS_test_label_size,
          FLAGS_test_thread_size),
    infer_(&data_, &model_, &loss_, FLAGS_infer_file, FLAGS_infer_label_size),
    epoch_(0), log_interval_(FLAGS_driver_log_interval),
    log_time_point_(::std::chrono::steady_clock::now()),
    checkpoint_interval_(FLAGS_driver_checkpoint_interval),
    checkpoint_time_point_(::std::chrono::steady_clock::now()) {
  if (data_.rewind() == false) {
    LOG(FATAL) << "Data cannot open data file " << FLAGS_data_file;
  }
  if (FLAGS_joe_task == "train") {
    if (FLAGS_driver_resume == true) {
      LOG(INFO) << "Driver resume from checkpoint at "
                << FLAGS_driver_location;
      resume();
    } else {
      LOG(INFO) << "Driver initialize model with mu = " << FLAGS_model_mu
                << " and sigma = " << FLAGS_model_sigma;
      model_.initialize(static_cast< value_type >(FLAGS_model_mu),
                        static_cast< value_type >(FLAGS_model_sigma));
    }
  }
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::runTrain() {
  for (; epoch_ < FLAGS_driver_epoch_size; ++epoch_) {
    LOG(INFO) << "Driver start training for epoch " << (epoch_ + 1);
    train_.train([&](const train_local &local) -> void {trainCallback(local);});
    train_.join();
    LOG(INFO) << "Driver finish training for epoch " << (epoch_ + 1);
    data_.rewind();
    if (FLAGS_driver_save > 0 && (epoch_ + 1) % FLAGS_driver_save == 0) {
      LOG(INFO) << "Driver save model for epoch " << (epoch_ + 1);
      save();
    }
  }
  LOG(INFO) << "Driver save checkpoint to " << FLAGS_driver_location;
  checkpoint();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::runTest() {
  using namespace ::std::filesystem;
  using ::thunder::FileBinarySerializer;

  path model_path = path(FLAGS_driver_location).append(
      FLAGS_driver_model);
  LOG(INFO) << "Driver load inference model from " << model_path.string();
  FileBinarySerializer model_serializer(
      model_path.string(), FileBinarySerializer::in);
  model_serializer.load(&model_);

  LOG(INFO) << "Driver start testing on file " << FLAGS_data_file;
  test_.test([&](const test_local &local) -> void {testCallback(local);});
  test_.join();
  LOG(INFO) << "Driver finish testing, error = " << test_.error() <<
      ", objective = " << test_.objective();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::runInfer() {
  using namespace ::std::filesystem;
  using ::thunder::FileBinarySerializer;

  path model_path = path(FLAGS_driver_location).append(
      FLAGS_driver_model);
  LOG(INFO) << "Driver load inference model from " << model_path.string();
  FileBinarySerializer model_serializer(
      model_path.string(), FileBinarySerializer::in);
  model_serializer.load(&model_);

  LOG(INFO) << "Driver start inference on " << FLAGS_data_file;
  LOG(INFO) << "Driver inference result is written to " << FLAGS_infer_file;
  if (infer_.infer([&](const infer_local &local) -> void {
                     inferCallback(local);}) == false) {
    LOG(FATAL) << "Driver inference failure";
  }
  LOG(INFO) << "Driver finish inference";
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::checkpoint() {
  using namespace ::std::filesystem;
  using ::thunder::FileBinarySerializer;

  ::std::lock_guard< ::std::mutex > lock(checkpoint_mutex_);
  ::std::error_code ec;

  // Lock the training process
  train_.lock();

  // Serialize the record
  path driver_path = path(FLAGS_driver_location).append("driver.tdb");
  path driver_backup_path = path(FLAGS_driver_location).append(
      "driver.tdb.backup");
  rename(driver_path, driver_backup_path, ec);
  FileBinarySerializer driver_serializer(
      driver_path.string(), FileBinarySerializer::out);
  driver_serializer.save(data_.count());
  driver_serializer.save(data_.offset());
  driver_serializer.save(train_.step());
  driver_serializer.save(epoch_);

  // Serialize the model
  path model_path = path(FLAGS_driver_location).append("model.tdb");
  path model_backup_path = path(FLAGS_driver_location).append(
      "model.tdb.backup");
  rename(model_path, model_backup_path, ec);
  FileBinarySerializer model_serializer(
      model_path.string(), FileBinarySerializer::out);
  model_serializer.save(model_);

  // Unlock the training process
  train_.unlock();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::save() {
  using ::std::filesystem::path;
  using ::thunder::FileBinarySerializer;

  // Lock the training process
  train_.lock();

  // Serialize the model
  path model_path = path(FLAGS_driver_location).append(
      "model_" + ::std::to_string(epoch_ + 1) + ".tdb");
  FileBinarySerializer model_serializer(
      model_path.string(), FileBinarySerializer::out);
  model_serializer.save(model_);

  // Unlock the training process
  train_.unlock();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::resume() {
  using ::std::filesystem::path;
  using ::thunder::FileBinarySerializer;

  path driver_path = path(FLAGS_driver_location).append("driver.tdb");
  FileBinarySerializer driver_serializer(
      driver_path.string(), FileBinarySerializer::in);
  size_type data_count;
  driver_serializer.load(&data_count);
  long data_offset;
  driver_serializer.load(&data_offset);
  data_.seek(data_offset, data_count);
  size_type train_step;
  driver_serializer.load(&train_step);
  train_.set_step(train_step);
  driver_serializer.load(&epoch_);

  path model_path = path(FLAGS_driver_location).append("model.tdb");
  FileBinarySerializer model_serializer(
      model_path.string(), FileBinarySerializer::in);
  model_serializer.load(&model_);
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::trainCallback(const train_local &local) {
  // Log the information
  if (checkLog() == true) {
    trainLog(local);
  }

  // Check point
  if (checkCheckpoint() == true) {
    LOG(INFO) << "Train save checkpoint to " << FLAGS_driver_location;
    checkpoint();
  }
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::testCallback(const test_local &local) {
  // Log the information
  if (checkLog() == true) {
    testLog(local);
  }
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::inferCallback(const infer_local &local) {
  // Log the information
  if (checkLog() == true) {
    inferLog(local);
  }
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
bool Driver< D, U, M, L, T, V, I >::checkLog() {
  using ::std::chrono::steady_clock;

  bool result = false;
  if (log_interval_.count() >= 0.0) {
    log_time_mutex_.lock();
    duration_type diff = steady_clock::now() - log_time_point_;
    if (diff >= log_interval_) {
      log_time_point_ = steady_clock::now();
      result = true;
    }
    log_time_mutex_.unlock();
  }
  return result;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
bool Driver< D, U, M, L, T, V, I >::checkCheckpoint() {
  using ::std::chrono::steady_clock;

  bool result = false;
  if (checkpoint_interval_.count() >= 0.0) {
    checkpoint_time_mutex_.lock();
    duration_type diff = steady_clock::now() - checkpoint_time_point_;
    if (diff >= checkpoint_interval_) {
      checkpoint_time_point_ = steady_clock::now();
      result = true;
    }
    checkpoint_time_mutex_.unlock();
  }
  return result;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::trainLog(const train_local &local) {
  ::std::ostringstream message;
  message << ::std::setprecision(FLAGS_driver_log_precision)
          << "Train step = " << train_.step() << ", rate = " << train_.rate()
          << ", data_objective = " << local.data_objective
          << ", universum_objective = " << local.universum_objective;
  if (FLAGS_driver_debug == true) {
    // Log data
    const field_array &data_input = local.data_input;
    const index_pair &data_label = local.data_label;
    for (size_type i = 0; i < data_input.size(); ++i) {
      message << ", data_input[" << i << "] = " << fieldToString(data_input[i]);
    }
    message << ", data_label = " << data_label.first
            << ":" << data_label.second;
    // Log universum
    const field_array &universum_input = local.universum_input;
    const index_pair &universum_label = local.universum_label;
    for (size_type i = 0; i < universum_input.size(); ++i) {
      if (i < universum_input.size()) {
        message << ", universum_input[" << i << "] = " << fieldToString(
            universum_input[i]);
      }
    }
    message << ", universum_label = " << universum_label.first
            << ":" << universum_label.second;
    // Log model and loss data
    const tensor_array &input_embedding = local.model.input_embedding();
    const tensor_type &output_embedding = local.model.output_embedding();
    const tensor_type &output = local.model.output();
    const tensor_type &grad_output = local.loss.grad_input();
    const tensor_type &feature = local.model.feature();
    const tensor_type &grad_feature = local.model.grad_feature();
    for (size_type i = 0; i < input_embedding.size(); ++i) {
      message << ", input_embedding[" << i << "] = " << tensorToString(
          input_embedding[i]);
    }
    message << ", output_embedding = " << tensorToString(output_embedding)
            << ", output = " << tensorToString(output)
            << ", grad_output = " << tensorToString(grad_output)
            << ", feature = " << tensorToString(feature)
            << ", grad_feature = " << tensorToString(grad_feature);
  }
  LOG(INFO) << message.str();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::testLog(const test_local &local) {
  ::std::ostringstream message;
  message << ::std::setprecision(FLAGS_driver_log_precision)
          << "Test count = " << test_.count()
          << ", objective = " << test_.objective()
          << ", error = " << test_.error()
          << ", data_objective = " << local.data_objective
          << ", data_error = " << local.data_error
          << ", data_position = " << local.data_position(0);
  if (FLAGS_driver_debug == true) {
    // Log data
    const field_array &data_input = local.data_input;
    const index_pair &data_label = local.data_label;
    for (size_type i = 0; i < data_input.size(); ++i) {
      message << ", data_input[" << i << "] = " << fieldToString(data_input[i]);
    }
    message << ", data_label = " << data_label.first
            << ":" << data_label.second;
    // Log model and loss data
    const tensor_array &input_embedding = local.model.input_embedding();
    const tensor_type &output_embedding = local.model.output_embedding();
    const tensor_type &output = local.model.output();
    const tensor_type &feature = local.model.feature();
    for (size_type i = 0; i < input_embedding.size(); ++i) {
      message << ", input_embedding[" << i << "] = " << tensorToString(
          input_embedding[i]);
    }
    message << ", output_embedding = " << tensorToString(output_embedding)
            << ", output = " << tensorToString(output)
            << ", feature = " << tensorToString(feature);
  }
  LOG(INFO) << message.str();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
void Driver< D, U, M, L, T, V, I >::inferLog(const infer_local &local) {
  ::std::ostringstream message;
  message << ::std::setprecision(FLAGS_driver_log_precision)
          << "Infer count = " << infer_.count()
          << ", data_position = " << local.data_position(0);
  if (FLAGS_driver_debug == true) {
    // Log data
    const field_array &data_input = local.data_input;
    for (size_type i = 0; i < data_input.size(); ++i) {
      message << ", data_input[" << i << "] = " << fieldToString(data_input[i]);
    }
    // Log model and loss data
    const tensor_array &input_embedding = model_.input_embedding();
    const tensor_type &output_embedding = model_.output_embedding();
    const tensor_type &output = model_.output();
    const tensor_type &feature = model_.feature();
    for (size_type i = 0; i < input_embedding.size(); ++i) {
      message << ", input_embedding[" << i << "] = " << tensorToString(
          input_embedding[i]);
    }
    message << ", output_embedding = " << tensorToString(output_embedding)
            << ", output = " << tensorToString(output)
            << ", feature = " << tensorToString(feature);
  }
  LOG(INFO) << message.str();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
typename Driver< D, U, M, L, T, V, I >::format_array
Driver< D, U, M, L, T, V, I >::parseDataFormat() {
  format_array format;
  ::std::regex format_regex("kIndex|kBytes");
  for(::std::sregex_iterator match(
          FLAGS_data_format.begin(), FLAGS_data_format.end(), format_regex);
      match != ::std::sregex_iterator(); ++match) {
    if ((*match).str() == "kIndex") {
      format.push_back(kIndex);
    } else if ((*match).str() == "kBytes") {
      format.push_back(kBytes);
    }
  }
  return format;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
typename Driver< D, U, M, L, T, V, I >::size_storage
Driver< D, U, M, L, T, V, I >::parseModelInputSize() {
  ::std::vector< size_type > input_size;
  ::std::regex size_regex("\\d+");
  for(::std::sregex_iterator match(
          FLAGS_model_input_size.begin(), FLAGS_model_input_size.end(),
          size_regex); match != ::std::sregex_iterator(); ++match) {
    input_size.push_back(::std::strtoul((*match).str().c_str(), nullptr, 10));
  }
  size_storage input_size_storage(input_size.size());
  for (size_type i = 0; i < input_size.size(); ++i) {
    input_size_storage[i] = input_size[i];
  }
  return input_size_storage;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
typename Driver< D, U, M, L, T, V, I >::gram_array
Driver< D, U, M, L, T, V, I >::parseModelGram() {
  gram_array gram;
  ::std::regex field_regex("\\{[^\\{\\}]*\\}");
  ::std::regex gram_regex("\\d+(:\\d+)?(:\\d+)?");
  ::std::regex value_regex("\\d+");
  for (::std::sregex_iterator field_match(
           FLAGS_model_gram.begin(), FLAGS_model_gram.end(), field_regex);
       field_match != ::std::sregex_iterator(); ++field_match) {
    ::std::string field_string = (*field_match).str();
    size_array field_gram;
    for (::std::sregex_iterator gram_match(
             field_string.begin(), field_string.end(), gram_regex);
         gram_match != ::std::sregex_iterator(); ++gram_match) {
      ::std::string gram_string = (*gram_match).str();
      ::std::vector< size_type > values;
      for (::std::sregex_iterator value_match(
               gram_string.begin(), gram_string.end(), value_regex);
           value_match != ::std::sregex_iterator(); ++value_match) {
        values.push_back(::std::strtoul(
            (*value_match).str().c_str(), nullptr, 10));
      }
      if (values.size() == 1) {
        // a
        field_gram.push_back(values[0]);
      } else if (values.size() == 2) {
        // a:b
        for (size_type value = values[0]; value <= values[1]; ++value) {
          field_gram.push_back(value);
        }
      } else if (values.size() == 3) {
        // a:b:c
        for (size_type value = values[0]; value <= values[2];
             value = value + values[1]) {
          field_gram.push_back(value);
        }
      }
    }
    gram.push_back(field_gram);
  }
  return gram;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
::std::string Driver< D, U, M, L, T, V, I >::tensorToString(
     const tensor_type &t) {
  ::std::ostringstream stream;
  stream <<  ::std::setprecision(FLAGS_driver_log_precision)
         << "[" << t.size(0);
  for (size_type i = 1; i < t.dimension(); ++i) {
    stream << ", " << t.size(i);
  }
  stream << ", " << t.mean() << ", " << t.std() << ", " << t.min()
         << ", " << t.max() << "]";
  return stream.str();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
::std::string Driver< D, U, M, L, T, V, I >::fieldToString(
     const field_variant &f) {
  ::std::ostringstream stream;
  const index_array *field_index;
  const byte_array *field_bytes;
  if ((field_index = ::std::get_if< index_array >(&f)) != nullptr) {
    stream << "[kIndex, " << field_index->size();
    if (field_index->size() > 0) {
      stream << ", " << ::std::setprecision(FLAGS_driver_log_precision)
             << (*field_index)[0].first << ":" << (*field_index)[0].second;
    }
    for (size_type i = 1; i < field_index->size() &&
             i < FLAGS_driver_log_precision; ++i) {
      stream << "," << (*field_index)[i].first
             << ":" << (*field_index)[i].second;
    }
    stream << "]";
  } else if ((field_bytes = ::std::get_if< byte_array >(&f)) != nullptr) {
    stream << "[kBytes, " << field_bytes->size();
    if (field_bytes->size() > 0) {
      stream << ", " << ::std::setw(2) << ::std::setfill('0') << ::std::hex;
    }
    for (size_type i = 0; i < field_bytes->size() &&
             i < FLAGS_driver_log_precision; ++i) {
      stream << static_cast< uint64_t >((*field_bytes)[i]);
    }
    stream << "]";
  }
  return stream.str();
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
const D &Driver< D, U, M, L, T, V, I >::data() {
  return data_;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
const U &Driver< D, U, M, L, T, V, I >::universum() {
  return universum_;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
const M &Driver< D, U, M, L, T, V, I >::model() {
  return model_;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
const L &Driver< D, U, M, L, T, V, I >::loss() {
  return loss_;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
const T &Driver< D, U, M, L, T, V, I >::train() {
  return train_;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
const V &Driver< D, U, M, L, T, V, I >::test() {
  return test_;
}

template < typename D, typename U, typename M, typename L, typename T,
           typename V, typename I >
const I &Driver< D, U, M, L, T, V, I >::infer() {
  return infer_;
}

}  // namespace bytesteady
