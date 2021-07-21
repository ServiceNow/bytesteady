#!/bin/bash
# Script to execute or resume training
# Copyright 2019 Xiang Zhang
#
# Usage: bash train.sh [data] [model] [threads] ...

set -x;
set -e;

DATA="${1:-jointbinary}";
MODEL="${2:-doublefnvnllgram1a2a4a8a16size16777216dimension16a0.1b0alpha0lambda0.001n0rho0}";
THREADS="${3:-10}";

DIRECTORY=models/$DATA/$MODEL;
EXECUTABLE=train/$DATA\_$MODEL.sh;

# Load the envirionment
source /home/xiang/Downloads/bytesteady/setup.sh;

# Go to the task root
cd /home/xiang/Downloads/bytesteady/bytesteady/text_classification;

# If the training script does not exist, exit
if [ ! -e $EXECUTABLE ]; then
    exit;
fi;

# If the model directory does not exist, create it
if [ ! -d $DIRECTORY ]; then
    mkdir -p $DIRECTORY;
fi;

# If the file driver.tdb exists, then resume. Otherwise, start anew
if [ ! -e $DIRECTORY/driver.tdb ]; then
    bash $EXECUTABLE -driver_save 0 -train_thread_size $THREADS "${@:4}";
else
    bash $EXECUTABLE -driver_save 0 -driver_resume -train_thread_size $THREADS "${@:4}";
fi;
