#!/bin/bash

# Archived program command-line for experiment
# Copyright 2021 ServiceNow All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

../bytesteady/bytesteady -driver_location models/AudioMNIST/digitfreq1e1024split1doublefnvnllgram1a2a4a8a16size16777216dimension16a0.1b0alpha0lambda0.001n0rho0 -driver_epoch_size 1000 -data_file data/AudioMNIST/data/digitfreq1e1024split1train_shuffle.hex -data_format 'kBytes,kBytes,kBytes,kBytes,kBytes,kBytes,kBytes,kBytes,kBytes,kBytes,kBytes' -model_input_size 16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216 -model_output_size 11 -model_gram '{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16},{1,2,4,8,16}' -model_dimension 16 -train_a 0.1 -train_b 0 -train_alpha 0 -train_lambda 0.001 -train_n 0 -train_rho 0 "$@";
