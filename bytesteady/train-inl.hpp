/*
 * Coprygith 2019 Xiang Zhang All Rights Reserved.
 */

#include <memory>
#include <mutex>
#include <thread>

#include "bytesteady/train.hpp"

namespace bytesteady {

template < typename D, typename U, typename M, typename L >
Train< D, U, M, L >::Train(
    D *d, U *u, M *m, L *l, value_type a_val, value_type b_val,
    value_type alpha_val, value_type lambda_val, size_type n_val,
    value_type rho_val, size_type t, size_type s) :
    data_(d), universum_(u), model_(m), loss_(l), a_(a_val), b_(b_val),
    alpha_(alpha_val), lambda_(lambda_val), n_(n_val), rho_(rho_val),
    thread_size_(t), step_(s), input_size_(m->input_embedding_size()),
    label_size_(m->output_embedding_size()) {                  
  updateRate();
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::train(const callback_type &callback) {
  threads_.clear();
  mutexes_.clear();
  for (size_type i = 0; i < thread_size_; ++i) {
    mutexes_.push_back(::std::make_shared< ::std::mutex >());
    threads_.push_back(::std::thread(
        &Train::job, this, callback, mutexes_[i].get()));
  }
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::job(
    const callback_type &callback, ::std::mutex *mutex) {
  Local local{model_->clone(true)};
  M &model = local.model;
  L &loss = local.loss;
  field_array &data_input = local.data_input;
  index_pair &data_label = local.data_label;
  value_type &data_objective = local.data_objective;
  field_array &universum_input = local.universum_input;
  index_pair &universum_label = local.universum_label;
  value_type &universum_objective = local.universum_objective;

  // Get sample untill reaching end othe first epoch
  while (data_->getSample(&data_input, &data_label) == true) {
    mutex->lock();
    // Forward propagation
    const tensor_type &data_output = model.forward(data_input);
    data_objective = loss.forward(
        data_output, data_label.first) * data_label.second;
    // Backward propagation
    const tensor_type &data_grad_output =
        loss.backward(data_output, data_label.first);
    data_grad_output.mul(data_label.second);
    // Parameter update
    model.update(data_input, data_grad_output, rate_, lambda_);
    // Get universum sample
    for (size_type i = 0; i < n_ && universum_->getSample(
             input_size_, label_size_, data_input, data_label, &universum_input,
             &universum_label) == true; ++i) {
      // Forward propagation
      const tensor_type &universum_output = model.forward(universum_input);
      universum_objective = loss.forward(
          universum_output, universum_label.first) * universum_label.second;
      // Backward propagation
      const tensor_type &universum_grad_output =
          loss.backward(universum_output, universum_label.first);
      universum_grad_output.mul(universum_label.second);
      // Parameter update
      model.update(
          universum_input, universum_grad_output, rate_ * rho_, lambda_);
    }

    // Update step count and learning rate
    mutex->unlock();
    step_mutex_.lock();
    step_ = step_ + 1;
    updateRate();
    step_mutex_.unlock();

    // Execute callback
    callback(local);
  }
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::join() {
  for (::std::thread &thread : threads_) {
    thread.join();
  }
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::lock() {
  // Lock the data reading operation
  data_->lock();
  // Lock the threads
  for (const ::std::shared_ptr< ::std::mutex > &mutex : mutexes_) {
    mutex->lock();
  }
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::unlock() {
  // Unlock the data reading operation
  data_->unlock();
  // Unlock the threads
  for (const ::std::shared_ptr< ::std::mutex > &mutex : mutexes_) {
    mutex->unlock();
  }
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::updateRate() {
  value_type ratio = 1.0 / (1.0 + alpha_ *  static_cast< value_type >(step_));
  rate_ = ratio * a_ + (1.0 - ratio) * b_;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::value_type Train< D, U, M, L >::a() const {
  return a_;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_a(value_type a_val) {
  a_ = a_val;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::value_type Train< D, U, M, L >::b() const {
  return b_;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_b(value_type b_val) {
  b_ = b_val;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::value_type Train< D, U, M, L >::alpha() const {
  return alpha_;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_alpha(value_type alpha_val) {
  alpha_ = alpha_val;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::value_type Train< D, U, M, L >::lambda() const {
  return lambda_;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_lambda(value_type lambda_val) {
  lambda_ = lambda_val;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::value_type Train< D, U, M, L >::rho() const {
  return rho_;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::size_type Train< D, U, M, L >::n() const {
  return n_;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_n(size_type n_val) {
  n_ = n_val;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_rho(value_type rho_val) {
  rho_ = rho_val;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::size_type Train< D, U, M, L >::thread_size() const {
  return thread_size_;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_thread_size(size_type t) {
  thread_size_ = t;
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::size_type Train< D, U, M, L >::step() const {
  return step_;
}

template < typename D, typename U, typename M, typename L >
void Train< D, U, M, L >::set_step(size_type s) {
  step_ = s;
  updateRate();
}

template < typename D, typename U, typename M, typename L >
typename Train< D, U, M, L >::value_type Train< D, U, M, L >::rate() const {
  return rate_;
}

}  // namespace bytesteady
