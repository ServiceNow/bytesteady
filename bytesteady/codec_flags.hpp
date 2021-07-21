/*
 * Copyright 2021 ServiceNow
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
