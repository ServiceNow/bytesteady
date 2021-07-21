/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#ifndef BYTESTEADY_HINGE_LOSS_HPP_
#define BYTESTEADY_HINGE_LOSS_HPP_

#include "thunder/tensor.hpp"

namespace bytesteady {

template < typename T = ::thunder::DoubleTensor >
class HingeLoss {
 public:
  typedef T tensor_type;
  typedef typename T::value_type value_type;
  typedef typename T::size_type size_type;

  value_type forward(const T &input, size_type target);
  // Backward ssumes that the preceding forward was called.
  const T &backward(const T &input, size_type target);

  value_type output() const;
  const T &grad_input() const;

 private:
  value_type output_;
  T grad_input_;
  T scratch_;
};

typedef HingeLoss< ::thunder::DoubleTensor > DoubleHingeLoss;
typedef HingeLoss< ::thunder::FloatTensor > FloatHingeLoss;
// Already exists: typedef DoubleHingeLoss HingeLoss;

}  // namespace bytesteady

namespace bytesteady {

// Pre-compiled template class instantiation
extern template class HingeLoss< ::thunder::DoubleTensor >;
extern template class HingeLoss< ::thunder::FloatTensor >;

}  // namespace bytesteady

#endif  // BYTESTEADY_HINGE_LOSS_HPP_
