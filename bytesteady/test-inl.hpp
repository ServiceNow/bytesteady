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

#include "bytesteady/test.hpp"

#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>

namespace bytesteady {

template < typename D, typename M, typename L >
Test< D, M, L >::Test(D *d, M *m, L *l, size_type label_size_val, size_type t) :
    data_(d), model_(m), loss_(l), label_size_(label_size_val), thread_size_(t)
{}

template < typename D, typename M, typename L >
void Test< D, M, L >::test(const callback_type &callback) {
  threads_.clear();
  mutexes_.clear();
  for (size_type i = 0; i < thread_size_; ++i) {
    mutexes_.push_back(::std::make_shared< ::std::mutex >());
    threads_.push_back(::std::thread(
        &Test::job, this, callback, mutexes_[i].get()));
  }
}

template < typename D, typename M, typename L >
void Test< D, M, L >::job(const callback_type &callback, ::std::mutex *mutex) {
  Local local{model_->clone(), L(), field_array(), index_pair(0, 1.0), 0.0,
              0.0, size_tensor(1)};
  M &model = local.model;
  L &loss = local.loss;
  field_array &data_input = local.data_input;
  index_pair &data_label = local.data_label;
  value_type &data_error = local.data_error;
  value_type &data_objective = local.data_objective;
  size_tensor &data_position = local.data_position;

  value_type ratio;
  count_ = 0;
  objective_ = 0;
  error_ = 0;
  while (data_->getSample(&data_input, &data_label) == true) {
    // Forward propagation
    mutex->lock();
    const tensor_type &data_output = model.forward(data_input);
    data_objective = loss.forward(
        data_output, data_label.first) * data_label.second;
    data_output.narrow(0, 0, ::std::min(label_size_, data_output.size(0))).max(
        &data_position);
    data_error = data_position(0) == data_label.first ? 0.0 : 1.0;
    mutex->unlock();
    // Update objective, error and count
    count_mutex_.lock();
    ratio = static_cast< value_type >(count_) /
        static_cast< value_type >(count_ + 1);
    objective_ = ratio * objective_ + (1.0 - ratio) * data_objective;
    error_ = ratio * error_ + (1.0 - ratio) * data_error;
    ++count_;
    count_mutex_.unlock();
    // Execute callback
    callback(local);
  }
}

template < typename D, typename M, typename L >
void Test< D, M, L >::join() {
  for (::std::thread &thread : threads_) {
    thread.join();
  }
}

template < typename D, typename M, typename L >
typename Test< D, M, L >::size_type Test< D, M, L >::count() const {
  return count_;
}

template < typename D, typename M, typename L >
typename Test< D, M, L >::value_type Test< D, M, L >::objective() const {
  return objective_;
}

template < typename D, typename M, typename L >
typename Test< D, M, L >::value_type Test< D, M, L >::error() const {
  return error_;
}

template < typename D, typename M, typename L >
typename Test< D, M, L >::size_type Test< D, M, L >::label_size() const {
  return label_size_;
}

template < typename D, typename M, typename L >
void Test< D, M, L >::set_label_size(size_type label_size_val) {
  label_size_ = label_size_val;
}

}  // namespace bytesteady
