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

#ifndef BYTESTEADY_TRAIN_HPP_
#define BYTESTEADY_TRAIN_HPP_

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "bytesteady/data.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"
#include "bytesteady/universum.hpp"

namespace bytesteady {

template < typename D = Data<>, typename U = Universum<>, typename M = Model<>,
           typename L = NLLLoss<> >
class Train {
 public:
  typedef D data_type;
  typedef U universum_type;
  typedef M model_type;
  typedef L loss_type;
  typedef typename D::field_array field_array;
  typedef typename D::index_pair index_pair;
  typedef typename M::size_storage size_storage;
  typedef typename M::size_type size_type;
  typedef typename M::tensor_type tensor_type;
  typedef typename M::value_type value_type;

  struct Local {
    M model;
    L loss;
    field_array data_input;
    index_pair data_label = {0, 1.0};
    value_type data_objective = 0.0;
    field_array universum_input;
    index_pair universum_label = {0, 1.0};
    value_type universum_objective = 0.0;
  };
  typedef Local local_type;
  typedef ::std::function< void (const Local &) > callback_type;

  // a: initial learning rate
  // b: eventual learning rate
  // alpha: learning rate change factor
  // lambda: weight decay factor
  // n: number of universum samples per data sample
  // rho: universum learning rate factor
  Train(D *d, U *u, M *m, L *l, value_type a_val = 1e-2, value_type b_val = 0.0,
        value_type alpha_val = 0.0, value_type lambda_val = 1e-5,
        size_type n_val = 0, value_type rho_val = 0.0, size_type t = 1,
        size_type s = 0);

  // Guarantee: when callback() is called, no mutex will be held by the thread.
  void train(const callback_type &callback);
  void job(const callback_type &callback, ::std::mutex *mutex);

  void join();
  void lock();
  void unlock();

  void updateRate();

  value_type a() const;
  void set_a(value_type a_val);

  value_type b() const;
  void set_b(value_type b_val);

  value_type alpha() const;
  void set_alpha(value_type alpha_val);

  value_type lambda() const;
  void set_lambda(value_type lambda_val);

  size_type n() const;
  void set_n(size_type n_val);

  value_type rho() const;
  void set_rho(value_type rho_val);

  size_type thread_size() const;
  void set_thread_size(size_type t);

  size_type step() const;
  void set_step(size_type s);

  value_type rate() const;

 private:
  // Foreign object pointers
  D *data_;
  U *universum_;
  M *model_;
  L *loss_;

  // Member variables
  value_type a_;
  value_type b_;
  value_type alpha_;
  value_type lambda_;
  size_type n_;
  value_type rho_;
  size_type thread_size_;

  size_type step_;
  value_type rate_;
  size_storage input_size_;
  size_type label_size_;

  // Thread and mutex container
  ::std::vector< ::std::thread > threads_;
  ::std::vector< ::std::shared_ptr< ::std::mutex > > mutexes_;

  // Mutex to update step and rate
  ::std::mutex step_mutex_;
};

typedef Train< DoubleData, DoubleUniversum, DoubleFNVModel, DoubleNLLLoss >
DoubleFNVNLLTrain;
typedef Train< FloatData, FloatUniversum, FloatFNVModel, FloatNLLLoss >
FloatFNVNLLTrain;
typedef Train< DoubleData, DoubleUniversum, DoubleCityModel, DoubleNLLLoss >
DoubleCityNLLTrain;
typedef Train< FloatData, FloatUniversum, FloatCityModel, FloatNLLLoss >
FloatCityNLLTrain;
typedef Train< DoubleData, DoubleUniversum, DoubleFNVModel, DoubleHingeLoss >
DoubleFNVHingeTrain;
typedef Train< FloatData, FloatUniversum, FloatFNVModel, FloatHingeLoss >
FloatFNVHingeTrain;
typedef Train< DoubleData, DoubleUniversum, DoubleCityModel, DoubleHingeLoss >
DoubleCityHingeTrain;
typedef Train< FloatData, FloatUniversum, FloatCityModel, FloatHingeLoss >
FloatCityHingeTrain;
// Already exists: typedef DoubleFNVNLLTrain Train;

}  // namespace bytesteady

namespace bytesteady {

// Pre-compiled template class instantiation
extern template class Train<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleNLLLoss >;
extern template class Train<
  FloatData, FloatUniversum, FloatFNVModel, FloatNLLLoss >;
extern template class Train<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleNLLLoss >;
extern template class Train<
  FloatData, FloatUniversum, FloatCityModel, FloatNLLLoss >;
extern template class Train<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleHingeLoss >;
extern template class Train<
  FloatData, FloatUniversum, FloatFNVModel, FloatHingeLoss >;
extern template class Train<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleHingeLoss >;
extern template class Train<
  FloatData, FloatUniversum, FloatCityModel, FloatHingeLoss >;

}  // namespace bytesteady

#endif  // BYTESTEADY_TRAIN_HPP_
