/*
 * Copyright 2019 Xiang Zhang All Rights Reserved
 */

#include "bytesteady/codec_driver.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <regex>
#include <string>
#include <system_error>
#include <variant>

#include "bytesteady/field_format.hpp"
#include "bytesteady/codec_flags.hpp"
#include "glog/logging.h"
#include "thunder/serializer.hpp"

namespace bytesteady {

template < typename D, typename B, typename C >
CodecDriver< D, B, C >::CodecDriver() :
    data_(FLAGS_data_file, parseDataFormat()),
    log_interval_(FLAGS_driver_log_interval),
    log_time_point_(::std::chrono::steady_clock::now()) {}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::runBuild() {
  codec_.clear();
  B builder(&data_, &codec_, parseBuilderGram());
  builder.build(
      [&](const builder_local &local) -> void {buildCallback(local);});
  save();
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::runEncode() {
  load();
  C coder(&data_, &codec_, FLAGS_coder_output, FLAGS_coder_thread_size);
  coder.encode([&](const coder_local &local) -> void {encodeCallback(local);});
  if (coder.join() != true) {
    LOG(FATAL) << "Driver error write to output " << FLAGS_coder_output;
  }
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::runDecode() {
  load();
  C coder(&data_, &codec_, FLAGS_coder_output, FLAGS_coder_thread_size);
  coder.decode([&](const coder_local &local) -> void {decodeCallback(local);});
  if (coder.join() != true) {
    LOG(FATAL) << "Driver error write to output " << FLAGS_coder_output;
  }
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::save() {
  using ::thunder::FileBinarySerializer;
  FileBinarySerializer codec_serializer(
      FLAGS_driver_file, FileBinarySerializer::out);
  codec_serializer.save(codec_.size());
  for (const typename codec_table::value_type &pair : codec_) {
    codec_serializer.save(pair.first);
    codec_serializer.save(pair.second);
  }
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::load() {
  using ::thunder::FileBinarySerializer;
  FileBinarySerializer codec_serializer(
      FLAGS_driver_file, FileBinarySerializer::in);
  codec_.clear();
  typename codec_table::size_type size;
  codec_serializer.load(&size);
  for (typename codec_table::size_type i = 0; i < size; ++i) {
    typename codec_table::key_type key;
    codec_serializer.load(&key);
    typename codec_table::mapped_type value;
    codec_serializer.load(&value);
    codec_[key] = value;
  }
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::buildCallback(const builder_local &local) {
  if (checkLog()) {
    buildLog(local);
  }
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::encodeCallback(const coder_local &local) {
  if (checkLog()) {
    codeLog(local, "Encode");
  }
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::decodeCallback(const coder_local &local) {
  if (checkLog()) {
    codeLog(local, "Decode");
  }
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::buildLog(const builder_local &local) {
  ::std::ostringstream message;
  message << ::std::setprecision(FLAGS_driver_log_precision)
          << "Build field = " << local.data_field << ", count = "
          << data_.count();
  if (FLAGS_driver_debug == true) {
    const field_array &data_input = local.data_input;
    const index_pair &data_label = local.data_label;
    for (size_type i = 0; i < data_input.size(); ++i) {
      message << ", data_input[" << i << "] = " << fieldToString(data_input[i]);
    }
    message << ", data_label = " << data_label.first
            << ":" << data_label.second;
  }
  LOG(INFO) << message.str();
}

template < typename D, typename B, typename C >
void CodecDriver< D, B, C >::codeLog(
    const coder_local &local, const ::std::string &task) {
  ::std::ostringstream message;
  message << ::std::setprecision(FLAGS_driver_log_precision)
          << task << " data_index = " << local.data_index;
  if (FLAGS_driver_debug == true) {
    const field_array &data_input = local.data_input;
    const index_pair &data_label = local.data_label;
    const field_array &data_output = local.data_output;
    for (size_type i = 0; i < data_input.size(); ++i) {
      message << ", data_input[" << i << "] = " << fieldToString(data_input[i]);
    }
    message << ", data_label = " << data_label.first
            << ":" << data_label.second;
    for (size_type i = 0; i < data_output.size(); ++i) {
      message << ", data_output[" << i << "] = "
              << fieldToString(data_output[i]);
    }
  }
  LOG(INFO) << message.str();
}

template < typename D, typename B, typename C >
bool CodecDriver< D, B, C >::checkLog() {
  using ::std::chrono::steady_clock;

  bool result = false;
  if (log_interval_.count() >= 0.0) {
    log_time_mutex_.lock();
    duration_type diff = steady_clock::now() - log_time_point_;
    if (diff >= log_interval_) {
      log_time_point_ = steady_clock::now();
      result = true;
    }
    log_time_mutex_.unlock();
  }
  return result;
}

template < typename D, typename B, typename C >
typename CodecDriver< D, B, C >::format_array
CodecDriver< D, B, C >::parseDataFormat() {
  format_array format;
  ::std::regex format_regex("kIndex|kBytes");
  for(::std::sregex_iterator match(
          FLAGS_data_format.begin(), FLAGS_data_format.end(), format_regex);
      match != ::std::sregex_iterator(); ++match) {
    if ((*match).str() == "kIndex") {
      format.push_back(kIndex);
    } else if ((*match).str() == "kBytes") {
      format.push_back(kBytes);
    }
  }
  return format;
}

template < typename D, typename B, typename C >
typename CodecDriver< D, B, C >::gram_array
CodecDriver< D, B, C >::parseBuilderGram() {
  gram_array gram;
  ::std::regex field_regex("\\{[^\\{\\}]*\\}");
  ::std::regex gram_regex("\\d+(:\\d+)?(:\\d+)?");
  ::std::regex value_regex("\\d+");
  for (::std::sregex_iterator field_match(
           FLAGS_builder_gram.begin(), FLAGS_builder_gram.end(), field_regex);
       field_match != ::std::sregex_iterator(); ++field_match) {
    ::std::string field_string = (*field_match).str();
    size_array field_gram;
    for (::std::sregex_iterator gram_match(
             field_string.begin(), field_string.end(), gram_regex);
         gram_match != ::std::sregex_iterator(); ++gram_match) {
      ::std::string gram_string = (*gram_match).str();
      ::std::vector< size_type > values;
      for (::std::sregex_iterator value_match(
               gram_string.begin(), gram_string.end(), value_regex);
           value_match != ::std::sregex_iterator(); ++value_match) {
        values.push_back(::std::strtoul(
            (*value_match).str().c_str(), nullptr, 10));
      }
      if (values.size() == 1) {
        // a
        field_gram.push_back(values[0]);
      } else if (values.size() == 2) {
        // a:b
        for (size_type value = values[0]; value <= values[1]; ++value) {
          field_gram.push_back(value);
        }
      } else if (values.size() == 3) {
        // a:b:c
        for (size_type value = values[0]; value <= values[2];
             value = value + values[1]) {
          field_gram.push_back(value);
        }
      }
    }
    gram.push_back(field_gram);
  }
  return gram;
}

template < typename D, typename B, typename C >
::std::string CodecDriver< D, B, C >::fieldToString(
     const field_variant &f) {
  ::std::ostringstream stream;
  const index_array *field_index;
  const byte_array *field_bytes;
  if ((field_index = ::std::get_if< index_array >(&f)) != nullptr) {
    stream << "[kIndex, " << field_index->size();
    if (field_index->size() > 0) {
      stream << ", " << ::std::setprecision(FLAGS_driver_log_precision)
             << (*field_index)[0].first << ":" << (*field_index)[0].second;
    }
    for (size_type i = 1; i < field_index->size() &&
             i < FLAGS_driver_log_precision; ++i) {
      stream << "," << (*field_index)[i].first
             << ":" << (*field_index)[i].second;
    }
    stream << "]";
  } else if ((field_bytes = ::std::get_if< byte_array >(&f)) != nullptr) {
    stream << "[kBytes, " << field_bytes->size();
    if (field_bytes->size() > 0) {
      stream << ", " << ::std::setw(2) << ::std::setfill('0') << ::std::hex;
    }
    for (size_type i = 0; i < field_bytes->size() &&
             i < FLAGS_driver_log_precision; ++i) {
      stream << static_cast< uint64_t >((*field_bytes)[i]);
    }
    stream << "]";
  }
  return stream.str();
}

}  // namespace bytesteady
