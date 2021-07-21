/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_INFER_HPP_
#define BYTESTEADY_INFER_HPP_

#include <cstdio>
#include <functional>
#include <limits>
#include <string>

#include "bytesteady/data.hpp"
#include "bytesteady/loss.hpp"
#include "bytesteady/model.hpp"
#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename D = Data<>, typename M = Model<>, typename L = NLLLoss<> >
class Infer {
 public:
  typedef D data_type;
  typedef M model_type;
  typedef L loss_type;
  typedef typename D::field_array field_array;
  typedef typename M::size_storage size_storage;
  typedef typename M::size_type size_type;
  typedef typename M::tensor_type tensor_type;
  typedef ::thunder::SizeTensor size_tensor;

  struct Local {
    field_array data_input;
    size_tensor data_position;
  };
  typedef Local local_type;
  typedef ::std::function< void (const Local &) > callback_type;

  Infer(D *d, M *m, L *l, const ::std::string &fn, size_type label_size_val =
        ::std::numeric_limits< size_type >::max());

  /*
   * Returns false if file cannot be opened or witten to.
   */
  bool infer(const callback_type &callback);

  size_type count() const;

  size_type label_size() const;
  void set_label_size(size_type label_size_val);

 private:
  D *data_;
  M *model_;
  L *loss_;
  ::std::string file_;
  size_type label_size_;
  size_storage size_;

  size_type count_;
  FILE *fp_;
};

typedef Infer< DoubleData, DoubleFNVModel, DoubleNLLLoss > DoubleFNVNLLInfer;
typedef Infer< FloatData, FloatFNVModel, FloatNLLLoss > FloatFNVNLLInfer;
typedef Infer< DoubleData, DoubleCityModel, DoubleNLLLoss > DoubleCityNLLInfer;
typedef Infer< FloatData, FloatCityModel, FloatNLLLoss > FloatCityNLLInfer;
typedef Infer< DoubleData, DoubleFNVModel, DoubleHingeLoss >
DoubleFNVHingeInfer;
typedef Infer< FloatData, FloatFNVModel, FloatHingeLoss > FloatFNVHingeInfer;
typedef Infer< DoubleData, DoubleCityModel, DoubleHingeLoss >
DoubleCityHingeInfer;
typedef Infer< FloatData, FloatCityModel, FloatHingeLoss > FloatCityHingeInfer;

}  // namespace bytesteady

namespace bytesteady {

// Pre-compiled template class instantiation
extern template class Infer< DoubleData, DoubleFNVModel, DoubleNLLLoss >;
extern template class Infer< FloatData, FloatFNVModel, FloatNLLLoss >;
extern template class Infer< DoubleData, DoubleCityModel, DoubleNLLLoss >;
extern template class Infer< FloatData, FloatCityModel, FloatNLLLoss >;
extern template class Infer< DoubleData, DoubleFNVModel, DoubleHingeLoss >;
extern template class Infer< FloatData, FloatFNVModel, FloatHingeLoss >;
extern template class Infer< DoubleData, DoubleCityModel, DoubleHingeLoss >;
extern template class Infer< FloatData, FloatCityModel, FloatHingeLoss >;

}  // namespace bytesteady

#endif  // BYINFEREADY_INFER_HPP_
