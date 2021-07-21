/*
 * Copyright 2019 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/codec_flags.hpp"

#include "gflags/gflags.h"

DEFINE_string(data_file, "bytesteady/unittest_train.txt",
              "data input file name");
DEFINE_string(data_format, "kBytes,kIndex", "a comma-separated list of kBytes"
              " or kIndex representing field types");

DEFINE_string(builder_gram, "{1,2,4},{}", "list of grams for each field");

DEFINE_string(coder_output, "output.txt", "output file");
DEFINE_int64(coder_thread_size, 1, "number of threads for encoding/decoding");

DEFINE_string(driver_file, "codec.tdb", "codec file");
DEFINE_bool(driver_debug, false,
            "whether to log in verbose mode for debugging");
DEFINE_double(driver_log_interval, 5.0, "time interval for logging");
DEFINE_int64(driver_log_precision, 4, "numerical precision for logging");

DEFINE_string(joe_task, "build", "task to run, can be build, encode or decode");
DEFINE_string(joe_codec, "huffman", "codec type, can be bytepair, digram "
              "huffman, bytehuffman or subsample");
