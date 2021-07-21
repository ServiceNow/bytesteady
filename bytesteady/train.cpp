/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/train.hpp"

#include "bytesteady/data.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"
#include "bytesteady/universum.hpp"

#include "bytesteady/train-inl.hpp"

namespace bytesteady {
// Pre-compiled template class instantiation
template class Train<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleNLLLoss >;
template class Train<
  FloatData, FloatUniversum, FloatFNVModel, FloatNLLLoss >;
template class Train<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleNLLLoss >;
template class Train<
  FloatData, FloatUniversum, FloatCityModel, FloatNLLLoss >;
template class Train<
  DoubleData, DoubleUniversum, DoubleFNVModel, DoubleHingeLoss >;
template class Train<
  FloatData, FloatUniversum, FloatFNVModel, FloatHingeLoss >;
template class Train<
  DoubleData, DoubleUniversum, DoubleCityModel, DoubleHingeLoss >;
template class Train<
  FloatData, FloatUniversum, FloatCityModel, FloatHingeLoss >;
}  // namespace bytesteady
