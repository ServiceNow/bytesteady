/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
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
