#!/bin/bash

# Archived program command-line for experiment
# Copyright 2021 ServiceNow All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

../bytesteady/bytesteady -driver_location models/index/doublefnvnllgram1a2a4a8a16a32a64size16777216dimension16a0.001b0alpha0lambda0n0rho0 -driver_epoch_size 100 -data_file data/index/train.bytesteady -data_format kBytes,kIndex -model_input_size 16777216,1024 -model_output_size 3 -model_gram '{1,2,4,8,16,32,64}' -model_dimension 16 -train_a 0.001 -train_b 0 -train_alpha 0 -train_lambda 0 -train_n 0 -train_rho 0 "$@";
