#!/bin/bash

# Archived program command-line for experiment
# Copyright 2019 Xiang Zhang All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

LAMBDA="${1:-0.001}"

LOCATION=models/ifeng/doublefnvnllgram1a2a4a8a16size16777216dimension16a0.1b0alpha0lambda${LAMBDA}n0rho0split

# Load the environment
source /home/xiang/Downloads/bytesteady/setup.sh;

# Go to the training root
cd /home/xiang/Downloads/bytesteady/bytesteady/text_classification;

# If the model directory does not exist, create it
if [ ! -d ${LOCATION} ]; then
    mkdir -p ${LOCATION};
fi;

# If the file driver.tdb exists, then resume. Otherwise, start anew
if [ -e ${LOCATION}/driver.tdb ]; then
    BYTESTEADYBIN="../bytesteady/bytesteady -driver_resume";
else
    BYTESTEADYBIN="../bytesteady/bytesteady";
fi;

${BYTESTEADYBIN} -driver_location ${LOCATION} -driver_epoch_size 100 -driver_save 0 -data_file data/ifeng/train_shuffle_split_0.hex -data_format kBytes -model_gram '{1,2,4,8,16}' -model_input_size 16777216 -model_output_size 6 -model_dimension 16 -train_a 0.1 -train_b 0 -train_alpha 0 -train_lambda ${LAMBDA} -train_n 0 -train_rho 0 "${@:2}";
