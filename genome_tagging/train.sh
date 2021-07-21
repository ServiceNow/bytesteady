#!/bin/bash
# Script to execute or resume training
# Copyright 2019 Xiang Zhang
#
# Usage: bash train.sh [data] [model] [threads] ...

set -x;
set -e;

DATA="${1:-index}";
MODEL="${2:-doublefnvnllgram16size16777216dimension16a0.1b0alpha0lambda0n0rho0}";
THREADS="${3:-10}";

DIRECTORY=models/$DATA/$MODEL;
EXECUTABLE=train/$DATA\_$MODEL.sh;

# Load the envirionment
source /home/xiang/Downloads/bytesteady/setup.sh;

# Go to the task root
cd /home/xiang/Downloads/bytesteady/bytesteady/genome_tagging;

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
