#!/bin/bash

# Archived program command-line for experiment
# Copyright 2019 Xiang Zhang All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

../bytesteady/bytesteady -driver_location models/dianping/huffman8doublefnvnllgram1a2a4a8a16size134217728dimension16a0.1b0alpha0lambda0n0rho0 -driver_epoch_size 100 -driver_save 10 -data_file data/dianping/train_shuffle_split_0_encode_gram8.hex -data_format kBytes -model_gram '{1,2,4,8,16}' -model_input_size 134217728 -model_output_size 3 -model_dimension 16 -train_a 0.1 -train_b 0 -train_alpha 0 -train_lambda 0 -train_n 0 -train_rho 0 "$@";
