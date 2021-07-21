/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/model.hpp"

#include <utility>
#include <variant>

#include "bytesteady/integer.hpp"
#include "thunder/random.hpp"
#include "thunder/serializer.hpp"

namespace bytesteady{

template < typename T, typename H >
Model< T, H >::Model(
    const size_storage &s, size_type c, size_type d, const gram_array &g,
    uint64_t sd) :
    input_embedding_(s.size()), output_embedding_(c, d), gram_(g), seed_(sd) {
  for (size_type i = 0; i < s.size(); ++i) {
    input_embedding_[i] = T(s[i], d);
  }
}

template < typename T, typename H >
Model< T, H >::Model(
    const tensor_array &ie, const T &oe, const gram_array &g, uint64_t sd) :
    input_embedding_(ie), output_embedding_(oe), gram_(g), seed_(sd) {}

template < typename T, typename H >
void Model< T, H >::initialize(value_type mu, value_type sigma) const {
  ::thunder::Random< T > random;
  for (const T &e : input_embedding_) {
    random.normal(e, mu, sigma);
  }
  random.normal(output_embedding_, mu, sigma);
}

template < typename T, typename H >
Model< T, H > Model< T, H >::clone(bool share) const {
  if (share == true) {
    return Model(input_embedding_, output_embedding_, gram_, seed_);
  } else {
    Model model(input_embedding_size(), output_embedding_size(), dimension(),
                gram_, seed_);
    for (size_type i = 0; i < input_embedding_.size(); ++i) {
      model.input_embedding_[i].copy(input_embedding_[i]);
    }
    model.output_embedding_.copy(output_embedding_);
    return model;
  }
}

template < typename T, typename H >
const T &Model< T, H >::forward(const field_array &input) {
  const index_array *field_index;
  const byte_array *field_bytes;
  feature_.resize(output_embedding_.size(1)).zero();
  // Loop over input fields to update the feature
  for (size_type i = 0; i < input.size(); ++i) {
    if ((field_index = ::std::get_if< index_array >(&input[i])) != nullptr) {
      // The field is an index array
      for (const index_pair &pair : *field_index) {
        linalg_.axpy(input_embedding_[i][pair.first], feature_, pair.second);
      }
    } else if ((field_bytes = ::std::get_if< byte_array >(
        &input[i])) != nullptr) {
      // The field is a byte sequence
      const size_array &field_gram = gram_[i];
      size_type field_size = 0;
      for (const size_type &g : field_gram) {
        field_size = field_size + (
            field_bytes->size() >= g ? (field_bytes->size() - g + 1) : 0);
      }
      value_type weight = 1.0 / static_cast< value_type >(field_size);
      for (const size_type &g : field_gram) {
        for (size_type j = 0; field_bytes->size() >= g &&
                 j < field_bytes->size() - g + 1; ++j) {
          size_type index = hash_.hash64(&(*field_bytes)[j], g, seed_) %
              input_embedding_[i].size(0);
          linalg_.axpy(input_embedding_[i][index], feature_, weight);
        }
      }
    }
  }
  // Update the output
  linalg_.gemv(output_embedding_, feature_,
               output_.resize(output_embedding_.size(0)).zero());
  return output_;
}

template < typename T, typename H >
void Model< T, H >::update(
    const field_array &input, const T &grad_output, value_type rate,
    value_type decay) {
  // Calculate grad_feature_ using gemv
  linalg_.gemv(
      scratch_.resize(
          output_embedding_.size(1), output_embedding_.size(0)).copy(
              output_embedding_.transpose(0, 1)),
      grad_output, grad_feature_.resize(feature_.size(0)).zero());

  // Loop over input fields to update input_embedding_
  const index_array *field_index;
  const byte_array *field_bytes;
  for (size_type i = 0; i < input.size(); ++i) {
    if ((field_index = ::std::get_if< index_array >(&input[i])) != nullptr) {
      // The field is an index array. Apply gradient update using axpy
      for (const index_pair &pair : *field_index) {
        linalg_.axpy(grad_feature_, input_embedding_[i][pair.first],
                     - rate * pair.second);
        // Apply weight decay only for activated embedding
        if (decay != 0.0) {
          linalg_.scal(input_embedding_[i][pair.first],
                       1.0 - pair.second * decay * rate);
        }
      }
    } else if ((field_bytes = ::std::get_if< byte_array >(
        &input[i])) != nullptr) {
      const size_array &field_gram = gram_[i];
      size_type field_size = 0;
      for (const size_type &g : field_gram) {
        field_size = field_size + (
            field_bytes->size() >= g ? (field_bytes->size() - g + 1) : 0);
      }
      value_type weight = 1.0 / static_cast< value_type >(field_size);
      for (const size_type &g : field_gram) {
        for (size_type j = 0; field_bytes->size() >= g &&
                 j < field_bytes->size() - g + 1; ++j) {
          size_type index = hash_.hash64(&(*field_bytes)[j], g, seed_) %
              input_embedding_[i].size(0);
          // Apply gradient update using axpy
          linalg_.axpy(
              grad_feature_, input_embedding_[i][index], -weight * rate);
          // Apply weight decay only for activated embedding
          if (decay != 0.0) {
            linalg_.scal(input_embedding_[i][index],
                         1.0 - weight * decay * rate);
          }
        }
      }
    }
  }

  // Apply gradient update for output_embedding_ using ger
  linalg_.ger(
      grad_output, feature_, output_embedding_, -rate);
  // Apply weight decay for output_embedding_
  if (decay != 0.0) {
    linalg_.scal(output_embedding_, 1.0 - decay * rate);
  }
}

template < typename T, typename H >
typename Model< T, H >::size_type Model< T, H >::input_size() const {
  return input_embedding_.size();
}

template < typename T, typename H >
typename Model< T, H >::size_storage
Model< T, H >::input_embedding_size() const {
  size_storage s(input_embedding_.size());
  for (size_type i = 0; i < input_embedding_.size(); ++i) {
    s[i] = input_embedding_[i].size(0);
  }
  return s;
}

template < typename T, typename H >
typename Model< T, H >::size_type Model< T, H >::input_embedding_size(
    size_type ind) const {
  return input_embedding_[ind].size(0);
}

template < typename T, typename H >
typename Model< T, H >::size_type
    Model< T, H >::output_embedding_size() const {
  return output_embedding_.size(0);
}

template < typename T, typename H >
typename Model< T, H >::size_type Model< T, H >::dimension() const {
  return output_embedding_.size(1);
}

template < typename T, typename H >
const typename Model< T, H >::tensor_array &
Model< T, H >::input_embedding() const {
  return input_embedding_;
}

template < typename T, typename H >
void Model< T, H >::set_input_embedding(const tensor_array &e) {
  input_embedding_ = e;
}

template < typename T, typename H >
const T &Model< T, H >::output_embedding() const {
  return output_embedding_;
}

template < typename T, typename H >
void Model< T, H >::set_output_embedding(const T &e) {
  output_embedding_ = e;
}

template < typename T, typename H >
const typename Model< T, H >::gram_array &Model< T, H >::gram() const {
  return gram_;
}

template < typename T, typename H >
void Model< T, H >::set_gram(const gram_array &g) {
  gram_ = g;
}

template < typename T, typename H >
uint64_t Model< T, H >::seed() const {
  return seed_;
}

template < typename T, typename H >
void Model< T, H >::set_seed(uint64_t sd) {
  seed_ = sd;
}

template < typename T, typename H >
const T &Model< T, H >::feature() const {
  return feature_;
}

template < typename T, typename H >
void Model< T, H >::set_feature(const T &e) {
  feature_ = e;
}

template < typename T, typename H >
const T &Model< T, H >::grad_feature() const {
  return grad_feature_;
}

template < typename T, typename H >
void Model< T, H >::set_grad_feature(const T &e) {
  grad_feature_ = e;
}

template < typename T, typename H >
const T &Model< T, H >::output() const {
  return output_;
}

template < typename T, typename H >
void Model< T, H >::set_output(const T &e) {
  output_ = e;
}

}  // namespace bytesteady


namespace thunder {
namespace serializer {

template < typename S, typename T, typename H >
void save(S *s, const ::bytesteady::Model< T, H > &m) {
  typedef typename ::bytesteady::Model< T, H >::gram_array gram_array;
  typedef typename ::bytesteady::Model< T, H >::size_array size_array;
  typedef typename ::bytesteady::Model< T, H >::size_type size_type;
  typedef typename ::bytesteady::Model< T, H >::tensor_array tensor_array;
  // Save input embedding
  const tensor_array &input_embedding = m.input_embedding();
  s->save(input_embedding.size());
  for (size_type i = 0; i < input_embedding.size(); ++i) {
    s->save(input_embedding[i]);
  }
  // Save output embedding
  s->save(m.output_embedding());
  // Save gram
  const gram_array &gram = m.gram();
  s->save(gram.size());
  for (const size_array &field_gram : gram) {
    s->save(field_gram.size());
    for (const size_type &field_gram_value : field_gram) {
      s->save(field_gram_value);
    }
  }
  // Save seed
  s->save(m.seed());
}

template < typename S, typename T, typename H >
void load(S *s, bytesteady::Model< T, H > *m) {
  typedef typename ::bytesteady::Model< T, H >::gram_array gram_array;
  typedef typename ::bytesteady::Model< T, H >::size_array size_array;
  typedef typename ::bytesteady::Model< T, H >::size_type size_type;
  typedef typename ::bytesteady::Model< T, H >::tensor_array tensor_array;
  size_type input_size;
  // Load input embedding
  s->load(&input_size);
  tensor_array input_embedding(input_size);
  for (size_type i = 0; i < input_size; ++i) {
    s->load(&input_embedding[i]);
  }
  m->set_input_embedding(input_embedding);
  // Load output embedding
  T output_embedding;
  s->load(&output_embedding);
  m->set_output_embedding(output_embedding);
  // Load gram
  size_type gram_size;
  s->load(&gram_size);
  gram_array gram(gram_size);
  for (size_array &field_gram : gram) {
    size_type field_gram_size;
    s->load(&field_gram_size);
    field_gram.resize(field_gram_size);
    for (size_type &field_gram_value : field_gram) {
      s->load(&field_gram_value);
    }
  }
  m->set_gram(gram);
  // Load seed
  uint64_t seed;
  s->load(&seed);
  m->set_seed(seed);
}

}  // namespace serializer
}  // namespace thunder
