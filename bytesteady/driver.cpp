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
