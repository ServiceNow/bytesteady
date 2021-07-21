/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_DRIVER_HPP_
#define BYTESTEADY_DRIVER_HPP_

#include <chrono>
#include <mutex>
#include <string>

#include "bytesteady/data.hpp"
#include "bytesteady/field_format.hpp"
#include "bytesteady/flags.hpp"
#include "bytesteady/infer.hpp"
#include "bytesteady/integer.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"
#include "bytesteady/test.hpp"
#include "bytesteady/train.hpp"
#include "bytesteady/universum.hpp"

namespace bytesteady {

template < typename D = Data<>, typename U = Universum<>, typename M = Model<>,
           typename L = NLLLoss<>, typename T = Train<>, typename V = Test<>,
           typename I = Infer<> >
class Driver {
 public:
  typedef D data_type;
  typedef U universum_type;
  typedef M model_type;
  typedef L loss_type;
  typedef T train_type;
  typedef V test_type;
  typedef I infer_type;
  typedef typename D::byte_array byte_array;
  typedef typename D::field_array field_array;
  typedef typename D::field_variant field_variant;
  typedef typename D::format_array format_array;
  typedef typename D::index_array index_array;
  typedef typename D::index_pair index_pair;
  typedef typename M::gram_array gram_array;
  typedef typename M::size_array size_array;
  typedef typename M::size_storage size_storage;
  typedef typename M::size_type size_type;
  typedef typename M::tensor_array tensor_array;
  typedef typename M::tensor_type tensor_type;
  typedef typename M::value_type value_type;
  typedef typename T::local_type train_local;
  typedef typename V::local_type test_local;
  typedef typename I::local_type infer_local;
  typedef typename ::std::chrono::duration< double > duration_type;
  typedef typename ::std::chrono::steady_clock::time_point time_point;

  Driver();

  void runTrain();
  void runTest();
  void runInfer();

  void checkpoint();
  void save();
  void resume();

  void trainCallback(const train_local &local);
  void testCallback(const test_local &local);
  void inferCallback(const infer_local &local);

  bool checkLog();
  bool checkCheckpoint();

  void trainLog(const train_local &local);
  void testLog(const test_local &local);
  void inferLog(const infer_local &local);

  static format_array parseDataFormat();
  static size_storage parseModelInputSize();
  static gram_array parseModelGram();

  static ::std::string tensorToString(const tensor_type &t);
  static ::std::string fieldToString(const field_variant &f);

  const D &data();
  const U &universum();
  const M &model();
  const L &loss();
  const T &train();
  const V &test();
  const I &infer();

 private:
  D data_;
  U universum_;
  M model_;
  L loss_;
  T train_;
  V test_;
  I infer_;

  size_type epoch_;
  duration_type log_interval_;
  time_point log_time_point_;
  ::std::mutex log_time_mutex_;

  duration_type checkpoint_interval_;
  time_point checkpoint_time_point_;
  ::std::mutex checkpoint_time_mutex_;
  ::std::mutex checkpoint_mutex_;
};

// typedef definition
typedef Driver<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleNLLLoss,
  DoubleFNVNLLTrain, DoubleFNVNLLTest, DoubleFNVNLLInfer > DoubleFNVNLLDriver;
typedef Driver<
  FloatData, FloatUniversum, FloatFNVModel, FloatNLLLoss,
  FloatFNVNLLTrain, FloatFNVNLLTest, FloatFNVNLLInfer > FloatFNVNLLDriver;
typedef Driver<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleNLLLoss,
  DoubleCityNLLTrain, DoubleCityNLLTest, DoubleCityNLLInfer >
DoubleCityNLLDriver;
typedef Driver<
  FloatData, FloatUniversum, FloatCityModel, FloatNLLLoss,
  FloatCityNLLTrain, FloatCityNLLTest, FloatCityNLLInfer > FloatCityNLLDriver;
typedef Driver<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleHingeLoss,
  DoubleFNVHingeTrain, DoubleFNVHingeTest, DoubleFNVHingeInfer >
DoubleFNVHingeDriver;
typedef Driver<
  FloatData, FloatUniversum, FloatFNVModel, FloatHingeLoss,
  FloatFNVHingeTrain, FloatFNVHingeTest, FloatFNVHingeInfer >
FloatFNVHingeDriver;
typedef Driver<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleHingeLoss,
  DoubleCityHingeTrain, DoubleCityHingeTest, DoubleCityHingeInfer >
DoubleCityHingeDriver;
typedef Driver<
  FloatData, FloatUniversum, FloatCityModel, FloatHingeLoss,
  FloatCityHingeTrain, FloatCityHingeTest, FloatCityHingeInfer >
FloatCityHingeDriver;

}  // namespace bytesteady

namespace bytesteady {
// Pre-compiled template class instantiation
extern template class Driver<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleNLLLoss,
  DoubleFNVNLLTrain, DoubleFNVNLLTest, DoubleFNVNLLInfer >;
extern template class Driver<
  FloatData, FloatUniversum, FloatFNVModel, FloatNLLLoss,
  FloatFNVNLLTrain, FloatFNVNLLTest, FloatFNVNLLInfer >;
extern template class Driver<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleNLLLoss,
  DoubleCityNLLTrain, DoubleCityNLLTest, DoubleCityNLLInfer >;
extern template class Driver<
  FloatData, FloatUniversum, FloatCityModel, FloatNLLLoss,
  FloatCityNLLTrain, FloatCityNLLTest, FloatCityNLLInfer >;
extern template class Driver<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleHingeLoss,
  DoubleFNVHingeTrain, DoubleFNVHingeTest, DoubleFNVHingeInfer >;
extern template class Driver<
  FloatData, FloatUniversum, FloatFNVModel, FloatHingeLoss,
  FloatFNVHingeTrain, FloatFNVHingeTest, FloatFNVHingeInfer >;
extern template class Driver<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleHingeLoss,
  DoubleCityHingeTrain, DoubleCityHingeTest, DoubleCityHingeInfer >;
extern template class Driver<
  FloatData, FloatUniversum, FloatCityModel, FloatHingeLoss,
  FloatCityHingeTrain, FloatCityHingeTest, FloatCityHingeInfer >;
}  // namespace bytesteady

#endif  // BYTESTEADY_DRIVER_HPP_
