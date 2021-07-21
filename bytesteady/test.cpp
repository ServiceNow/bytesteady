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

#include "bytesteady/data.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"

#include "bytesteady/test-inl.hpp"

namespace bytesteady {

// Template class instantiation
template class Test< DoubleData, DoubleFNVModel, DoubleNLLLoss >;
template class Test< FloatData, FloatFNVModel, FloatNLLLoss >;
template class Test< DoubleData, DoubleCityModel, DoubleNLLLoss >;
template class Test< FloatData, FloatCityModel, FloatNLLLoss >;
template class Test< DoubleData, DoubleFNVModel, DoubleHingeLoss >;
template class Test< FloatData, FloatFNVModel, FloatHingeLoss >;
template class Test< DoubleData, DoubleCityModel, DoubleHingeLoss >;
template class Test< FloatData, FloatCityModel, FloatHingeLoss >;

}  // namespace bytesteady
