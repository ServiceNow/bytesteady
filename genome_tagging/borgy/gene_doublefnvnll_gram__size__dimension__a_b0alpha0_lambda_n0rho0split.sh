#!/bin/bash

# Archived program command-line for experiment
# Copyright 2019 Xiang Zhang All Rights Reserved
#
# Usage: bash {this_file} [additional_options]

set -x;
set -e;

GRAM="${1:-1a2a4a8a16}"
SIZE="${2:-16777216}"
DIMENSION="${3:-16}"
A="${4:-0.1}"
LAMBDA="${5:-0.001}"

LOCATION=models/gene/doublefnvnllgram${GRAM}size${SIZE}dimension${DIMENSION}a${A}b0alpha0lambda${LAMBDA}n0rho0split

# Load the environment
source /home/xiang/Downloads/bytesteady/setup.sh;

# Go to the training root
cd /home/xiang/Downloads/bytesteady/bytesteady/genome_tagging;

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
${BYTESTEADYBIN} -driver_location ${LOCATION} -driver_epoch_size 100 -driver_save 0 -data_file data/gene/train.bytesteady -data_format kBytes -model_gram "{${_GRAM}}" -model_input_size ${SIZE} -model_output_size 7 -model_dimension ${DIMENSION} -train_a ${A} -train_b 0 -train_alpha 0 -train_lambda ${LAMBDA} -train_n 0 -train_rho 0 "${@:6}";
