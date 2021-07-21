#!/bin/bash

# Archived program command-line for experiment
# Copyright 2019 Xiang Zhang All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

../bytesteady/bytesteady -driver_location models -driver_epoch_size 10 -data_file data/train_shuffle_split_0.hex -data_format kBytes -model_gram '{8}' -model_input_size 1048576 -model_output_size 3 -model_dimension 8 -train_a 1 -train_lambda 0.001 -train_thread_size 8 "$@";
