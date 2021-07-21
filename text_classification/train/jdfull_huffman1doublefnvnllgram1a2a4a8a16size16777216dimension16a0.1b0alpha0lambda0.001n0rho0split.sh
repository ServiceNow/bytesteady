#!/bin/bash

# Archived program command-line for experiment
# Copyright 2019 Xiang Zhang All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

../bytesteady/bytesteady -driver_location models/jdfull/huffman1doublefnvnllgram1a2a4a8a16size16777216dimension16a0.1b0alpha0lambda0.001n0rho0split -driver_epoch_size 100 -driver_save 10 -data_file data/jd/full_train_shuffle_split_0_encode_gram1.hex -data_format kBytes -model_gram '{1,2,4,8,16}' -model_input_size 16777216 -model_output_size 6 -model_dimension 16 -train_a 0.1 -train_b 0 -train_alpha 0 -train_lambda 0.001 -train_n 0 -train_rho 0 "$@";
