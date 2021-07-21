#!/bin/bash

# Archived program command-line for experiment
# Copyright 2021 ServiceNow All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

TASK="${1:-digit}";
FREQ="${2:-32}";
SPLIT="${3:-1}";
GRAM="${4:-1a2a4a8a16}"
SIZE="${5:-16777216}";
DIMENSION="${6:-16}";
A="${7:-0.1}";
LAMBDA="${8:-0.001}";

LOCATION=models/AudioMNIST/${TASK}freq${FREQ}split${SPLIT}concatdoublefnvnllgram${GRAM}size${SIZE}dimension${DIMENSION}a${A}b0alpha0lambda${LAMBDA}n0rho0split

# Load the environment
source /home/xiang/Downloads/bytesteady/setup.sh;

# Go to the training root
cd /home/xiang/Downloads/bytesteady/bytesteady/audio_classification;

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

# Infer the correct output
OUTPUT="11";
if [ ${TASK} == "gender" ]; then
    OUTPUT="3";
fi;

# Parse gram configuration
_GRAM=${GRAM//a/,};
_GRAM=${_GRAM//t/:};
_GRAM=${_GRAM//s/:};
_GRAM=${_GRAM//f/\},\{};

${BYTESTEADYBIN} -driver_location ${LOCATION} -driver_epoch_size 1000 -driver_save 0 -data_file data/data/AudioMNIST/${TASK}freq${FREQ}split${SPLIT}train_shuffle_concat.hex -data_format kBytes -model_gram "{${_GRAM}}" -model_input_size ${SIZE} -model_output_size ${OUTPUT} -model_dimension ${DIMENSION} -train_a ${A} -train_b 0 -train_alpha 0 -train_lambda ${LAMBDA} -train_n 0 -train_rho 0 "${@:9}";
