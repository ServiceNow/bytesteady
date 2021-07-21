/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/infer.hpp"

#include "bytesteady/data.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"

#include "bytesteady/infer-inl.hpp"

namespace bytesteady {

// Template class instantiation
template class Infer< DoubleData, DoubleFNVModel, DoubleNLLLoss >;
template class Infer< FloatData, FloatFNVModel, FloatNLLLoss >;
template class Infer< DoubleData, DoubleCityModel, DoubleNLLLoss >;
template class Infer< FloatData, FloatCityModel, FloatNLLLoss >;
template class Infer< DoubleData, DoubleFNVModel, DoubleHingeLoss >;
template class Infer< FloatData, FloatFNVModel, FloatHingeLoss >;
template class Infer< DoubleData, DoubleCityModel, DoubleHingeLoss >;
template class Infer< FloatData, FloatCityModel, FloatHingeLoss >;

}  // namespace bytesteady
