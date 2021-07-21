#!/bin/bash

# Archived program command-line for experiment
# Copyright 2019 Xiang Zhang All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

DATA="${1:-dianping}"
OUTPUT="${2:-3}"
BYTEHUFFMAN="${3:-1}"
GRAM="${4:-1a2a4a8a16}"
SIZE="${5:-16777216}"
DIMENSION="${6:-16}"
A="${7:-0.1}"
LAMBDA="${8:-0.001}"

LOCATION=models/${DATA}/bytehuffman${BYTEHUFFMAN}doublefnvnllgram${GRAM}size${SIZE}dimension${DIMENSION}a${A}b0alpha0lambda${LAMBDA}n0rho0split

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

_GRAM=${GRAM//a/,}
_GRAM=${_GRAM//t/:}
_GRAM=${_GRAM//s/:}
_GRAM=${_GRAM//f/\},\{}
${BYTESTEADYBIN} -driver_location ${LOCATION} -driver_epoch_size 100 -driver_save 0 -data_file data/${DATA}/train_shuffle_split_0_encode_bytehuffman${BYTEHUFFMAN}.hex -data_format kBytes -model_gram "{${_GRAM}}" -model_input_size ${SIZE} -model_output_size ${OUTPUT} -model_dimension ${DIMENSION} -train_a ${A} -train_b 0 -train_alpha 0 -train_lambda ${LAMBDA} -train_n 0 -train_rho 0 "${@:9}";
