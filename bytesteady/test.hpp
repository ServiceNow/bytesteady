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

#ifndef BYTESTEADY_TEST_HPP_
#define BYTESTEADY_TEST_HPP_

#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>

#include "bytesteady/data.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"
#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename D = Data<>, typename M = Model<>, typename L = NLLLoss<> >
class Test {
 public:
  typedef D data_type;
  typedef M model_type;
  typedef L loss_type;
  typedef typename D::field_array field_array;
  typedef typename D::index_pair index_pair;
  typedef typename M::size_storage size_storage;
  typedef typename M::size_type size_type;
  typedef typename M::tensor_type tensor_type;
  typedef typename M::value_type value_type;
  typedef ::thunder::SizeTensor size_tensor;

  struct Local {
    M model;
    L loss;
    field_array data_input;
    index_pair data_label;
    value_type data_error;
    value_type data_objective;
    size_tensor data_position;
  };
  typedef Local local_type;
  typedef ::std::function< void (const Local &) > callback_type;

  Test(D *d, M *m, L *l, size_type label_size_val =
       ::std::numeric_limits< size_type >::max(), size_type t = 1);

  void test(const callback_type &callback);
  void job(const callback_type &callback, ::std::mutex *mutex);

  void join();

  size_type count() const;
  value_type objective() const;
  value_type error() const;

  size_type label_size() const;
  void set_label_size(size_type label_size_val);

 private:
  D *data_;
  M *model_;
  L *loss_;
  size_type label_size_;
  size_type thread_size_;

  size_type count_;
  value_type objective_;
  value_type error_;

  // Thread and mutex container
  ::std::vector< ::std::thread > threads_;
  ::std::vector< ::std::shared_ptr< ::std::mutex > > mutexes_;

  // Mutex to update count, objective and error
  ::std::mutex count_mutex_;
};

typedef Test< DoubleData, DoubleFNVModel, DoubleNLLLoss > DoubleFNVNLLTest;
typedef Test< FloatData, FloatFNVModel, FloatNLLLoss > FloatFNVNLLTest;
typedef Test< DoubleData, DoubleCityModel, DoubleNLLLoss > DoubleCityNLLTest;
typedef Test< FloatData, FloatCityModel, FloatNLLLoss > FloatCityNLLTest;
typedef Test< DoubleData, DoubleFNVModel, DoubleHingeLoss > DoubleFNVHingeTest;
typedef Test< FloatData, FloatFNVModel, FloatHingeLoss > FloatFNVHingeTest;
typedef Test< DoubleData, DoubleCityModel, DoubleHingeLoss >
DoubleCityHingeTest;
typedef Test< FloatData, FloatCityModel, FloatHingeLoss > FloatCityHingeTest;

}  // namespace bytesteady

namespace bytesteady {

// Pre-compiled template class instantiation
extern template class Test< DoubleData, DoubleFNVModel, DoubleNLLLoss >;
extern template class Test< FloatData, FloatFNVModel, FloatNLLLoss >;
extern template class Test< DoubleData, DoubleCityModel, DoubleNLLLoss >;
extern template class Test< FloatData, FloatCityModel, FloatNLLLoss >;
extern template class Test< DoubleData, DoubleFNVModel, DoubleHingeLoss >;
extern template class Test< FloatData, FloatFNVModel, FloatHingeLoss >;
extern template class Test< DoubleData, DoubleCityModel, DoubleHingeLoss >;
extern template class Test< FloatData, FloatCityModel, FloatHingeLoss >;

}  // namespace bytesteady

#endif  // BYTESTEADY_TEST_HPP_
