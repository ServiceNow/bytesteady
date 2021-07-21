/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/driver.hpp"

#include "bytesteady/data.hpp"
#include "bytesteady/infer.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"
#include "bytesteady/test.hpp"
#include "bytesteady/train.hpp"
#include "bytesteady/universum.hpp"

#include "bytesteady/driver-inl.hpp"

namespace bytesteady {
// Template class instantiation
template class Driver<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleNLLLoss,
  DoubleFNVNLLTrain, DoubleFNVNLLTest, DoubleFNVNLLInfer >;
template class Driver<
  FloatData, FloatUniversum, FloatFNVModel, FloatNLLLoss,
  FloatFNVNLLTrain, FloatFNVNLLTest, FloatFNVNLLInfer >;
template class Driver<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleNLLLoss,
  DoubleCityNLLTrain, DoubleCityNLLTest, DoubleCityNLLInfer >;
template class Driver<
  FloatData, FloatUniversum, FloatCityModel, FloatNLLLoss,
  FloatCityNLLTrain, FloatCityNLLTest, FloatCityNLLInfer >;
template class Driver<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleHingeLoss,
  DoubleFNVHingeTrain, DoubleFNVHingeTest, DoubleFNVHingeInfer >;
template class Driver<
  FloatData, FloatUniversum, FloatFNVModel, FloatHingeLoss,
  FloatFNVHingeTrain, FloatFNVHingeTest, FloatFNVHingeInfer >;
template class Driver<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleHingeLoss,
  DoubleCityHingeTrain, DoubleCityHingeTest, DoubleCityHingeInfer >;
template class Driver<
  FloatData, FloatUniversum, FloatCityModel, FloatHingeLoss,
  FloatCityHingeTrain, FloatCityHingeTest, FloatCityHingeInfer >;
}  // namespace bytesteady
