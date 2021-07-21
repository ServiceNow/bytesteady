/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_CODEC_FLAGS_HPP_
#define BYTESTEADY_CODEC_FLAGS_HPP_

#include "gflags/gflags.h"

DECLARE_string(data_file);
DECLARE_string(data_format);

DECLARE_string(builder_gram);

DECLARE_string(coder_output);
DECLARE_int64(coder_thread_size);

DECLARE_string(driver_file);
DECLARE_bool(driver_debug);
DECLARE_double(driver_log_interval);
DECLARE_int64(driver_log_precision);

DECLARE_string(joe_task);
DECLARE_string(joe_codec);

#endif  // BYTESTEADY_CODEC_FLAGS_HPP_
