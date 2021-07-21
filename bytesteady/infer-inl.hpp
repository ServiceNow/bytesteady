/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/infer.hpp"

#include <cstdio>
#include <algorithm>
#include <string>

namespace bytesteady {

template < typename D, typename M, typename L >
Infer< D, M, L >::Infer(
    D *d, M *m, L *l, const ::std::string &fn, size_type label_size_val) :
    data_(d), model_(m), loss_(l), file_(fn), label_size_(label_size_val),
    size_(m->input_embedding_size()), fp_(nullptr) {}

template < typename D, typename M, typename L >
bool Infer< D, M, L >::infer(const callback_type &callback) {
  // Open file and check
  fp_ = ::std::fopen(file_.c_str(), "w");
  if (fp_ == nullptr) {
    return false;
  }

  Local local;
  field_array &data_input = local.data_input;
  size_tensor &data_position = local.data_position;

  count_ = 0;
  while (data_->getSample(&data_input, nullptr) == true) {
    const tensor_type &data_output = model_->forward(data_input);
    data_output.narrow(0, 0, ::std::min(label_size_, data_output.size(0))).max(
        &data_position);
    // Write to file and check
    if (::std::fprintf(fp_, "%lu\n", data_position(0)) <= 0) {
      return false;
    }
    ++count_;
    callback(local);
  }

  // Close file and check
  if (::std::fclose(fp_) != 0) {
    return false;
  }
  fp_ = nullptr;

  return true;
}

template < typename D, typename M, typename L >
typename Infer< D, M, L >::size_type Infer< D, M, L >::count() const {
  return count_;
}

template < typename D, typename M, typename L >
typename Infer< D, M, L >::size_type Infer< D, M, L >::label_size() const {
  return label_size_;
}

template < typename D, typename M, typename L >
void Infer< D, M, L >::set_label_size(size_type label_size_val) {
  label_size_ = label_size_val;
}

}  // namespace bytesteady
