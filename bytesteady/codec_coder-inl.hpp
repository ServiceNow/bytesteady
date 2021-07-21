/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/codec_coder.hpp"

#include <cstdio>
#include <mutex>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include "bytesteady/integer.hpp"

namespace bytesteady {

template < typename D, typename C >
bool CodecCoder< D, C >::Node::operator==(const Node &b) const {
  return data_index == b.data_index;
}

template < typename D, typename C >
bool CodecCoder< D, C >::Node::operator!=(const Node &b) const {
  return data_index != b.data_index;
}

template < typename D, typename C >
bool CodecCoder< D, C >::Node::operator<(const Node &b) const {
  return data_index < b.data_index;
}

template < typename D, typename C >
bool CodecCoder< D, C >::Node::operator>(const Node &b) const {
  return data_index > b.data_index;
}

template < typename D, typename C >
bool CodecCoder< D, C >::Node::operator<=(const Node &b) const {
  return data_index <= b.data_index;
}

template < typename D, typename C >
bool CodecCoder< D, C >::Node::operator>=(const Node &b) const {
  return data_index >= b.data_index;
}

template < typename D, typename C >
CodecCoder< D, C >::CodecCoder(
    D *d, codec_table *c, const ::std::string &fn, size_type t) :
    data_(d), codec_(c), file_(fn), thread_size_(t), mutexes_(t), results_(t) {}

template < typename D, typename C >
CodecCoder< D, C >::~CodecCoder() {
  close();
}

template < typename D, typename C >
void CodecCoder< D, C >::encode(const callback_type &callback) {
  count_ = 0;
  threads_.clear();
  for (size_type i = 0; i < thread_size_; ++i) {
    threads_.push_back(::std::thread(
        &CodecCoder::encodeJob, this, callback, &mutexes_[i], results_[i]));
  }
}

template < typename D, typename C >
void CodecCoder< D, C >::decode(const callback_type &callback) {
  count_ = 0;
  threads_.clear();
  for (size_type i = 0; i < thread_size_; ++i) {
    threads_.push_back(::std::thread(
        &CodecCoder::decodeJob, this, callback, &mutexes_[i], results_[i]));
  }
}

template < typename D, typename C >
void CodecCoder< D, C >::encodeJob(
    const callback_type &callback, ::std::mutex *mutex,
    typename ::std::vector< bool >::reference result) {
  Local local;
  size_type &data_index = local.data_index;
  field_array &data_input = local.data_input;
  index_pair &data_label = local.data_label;
  field_array &data_output = local.data_output;

  byte_array output_field_bytes;
  result = true;
  while(data_->getSample(&data_input, &data_label, &data_index)) {
    mutex->lock();
    data_output.clear();
    for (size_type i = 0; i < data_input.size(); ++i) {
      if ((*codec_).find(i) != (*codec_).end()) {
        const byte_array &input_field_bytes =
            ::std::get< byte_array >(data_input[i]);
        (*codec_)[i].encode(input_field_bytes, &output_field_bytes);
        data_output.push_back(output_field_bytes);
      } else {
        data_output.push_back(data_input[i]);
      }
    }
    mutex->unlock();
    result = write(data_output, data_label, data_index);
    if (result == false) {
      return;
    }
    callback(local);
  }
}

template < typename D, typename C >
void CodecCoder< D, C >::decodeJob(
    const callback_type &callback, ::std::mutex *mutex,
    typename ::std::vector< bool >::reference result) {
  Local local;
  size_type &data_index = local.data_index;
  field_array &data_input = local.data_input;
  index_pair &data_label = local.data_label;
  field_array &data_output = local.data_output;

  byte_array output_field_bytes;
  result = true;
  while(data_->getSample(&data_input, &data_label, &data_index)) {
    mutex->lock();
    data_output.clear();
    for (size_type i = 0; i < data_input.size(); ++i) {
      if ((*codec_).find(i) != (*codec_).end()) {
        const byte_array &input_field_bytes =
            ::std::get< byte_array >(data_input[i]);
        (*codec_)[i].decode(input_field_bytes, &output_field_bytes);
        data_output.push_back(output_field_bytes);
      } else {
        data_output.push_back(data_input[i]);
      }
    }
    mutex->unlock();
    result = write(data_output, data_label, data_index);
    if (result == false) {
      return;
    }
    callback(local);
  }
}

template < typename D, typename C >
bool CodecCoder< D, C >::join() {
  bool result = true;
  for (size_type i = 0; i < thread_size_; ++i) {
    threads_[i].join();
    if (results_[i] == false) {
      result = false;
    }
  }
  return result;
}

template < typename D, typename C >
void CodecCoder< D, C >::lock() {
  // Lock the data reading operation
  data_->lock();
  // Lock the data writing operation
  write_mutex_.lock();
  // Lock the threads
  for (::std::mutex &mutex : mutexes_) {
    mutex.lock();
  }
}

template < typename D, typename C >
void CodecCoder< D, C >::unlock() {
  // Unlock the data reading operation
  data_->unlock();
  // Unlock the data writing operation
  write_mutex_.unlock();
  // Lock the threads
  for (::std::mutex &mutex : mutexes_) {
    mutex.unlock();
  }
}

template < typename D, typename C >
bool CodecCoder< D, C >::write(
    const field_array &data_output, const index_pair &data_label,
    size_type data_index) {
  ::std::lock_guard< ::std::mutex > lock(write_mutex_);

  if (fp_ == nullptr) {
    fp_ = ::std::fopen(file_.c_str(), "w");
  }
  if (fp_ == nullptr) {
    return false;
  }

  // Put the data in queue
  Node node{data_index, data_output, data_label};
  queue_.push(node);

  while (queue_.size() > 0 && queue_.top().data_index == count_) {
    const field_array &output = queue_.top().data_output;
    const index_pair &label = queue_.top().data_label;
    // Write output fields
    const index_array *field_index;
    const byte_array *field_bytes;
    for (const field_variant &field : output) {
      if ((field_index = ::std::get_if< index_array >(&field)) != nullptr) {
        // Write first pair
        if (field_index->size() > 0) {
          const index_pair &pair = (*field_index)[0];
          if (pair.second == 1.0) {
            ::std::fprintf(fp_, "%lu", pair.first);
          } else {
            ::std::fprintf(fp_, "%lu:%g", pair.first, pair.second);
          }
        }
        // Write second and later pairs
        for (size_type i = 1; i < field_index->size(); ++i) {
          const index_pair &pair = (*field_index)[i];
          if (pair.second == 1.0) {
            ::std::fprintf(fp_, ",%lu", pair.first);
          } else {
            ::std::fprintf(fp_, ",%lu:%g", pair.first, pair.second);
          }
        }
      } else if ((field_bytes = ::std::get_if< byte_array >(&field)) != nullptr) {
        for (const uint8_t &byte : (*field_bytes)) {
          ::std::fprintf(fp_, "%2.2x", byte);
        }
      }
      ::std::fprintf(fp_, " ");
    }

    // Write output label
    if (label.second == 1.0) {
      fprintf(fp_, "%lu\n", label.first);
    } else {
      fprintf(fp_, "%lu:%g\n", label.first, label.second);
    }

    // Pop the queue
    queue_.pop();
    count_ = count_ + 1;
  }

  return true;
}

template < typename D, typename C >
void CodecCoder< D, C >::close() {
  ::std::lock_guard< ::std::mutex > lock(write_mutex_);
  if (fp_ != nullptr) {
    ::std::fclose(fp_);
  }
  fp_ = nullptr;
}

template < typename D, typename C >
typename CodecCoder< D, C >::size_type CodecCoder< D, C >::count() {
  return count_;
}

}  // namespace bytesteady
