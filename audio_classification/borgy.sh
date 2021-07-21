#!/bin/bash
# Script to start borgy job
# Copyright 2021 ServiceNow
#
# Usage: bash borgy.sh [memory] [data] [model] ...

set -x;
set -e;

CPU="${1:-10}"
MEMORY="${2:-8}"
MODEL="${3:-audiomnist_task__freq__split_concatdoublefnvnll_gram__size__dimension__a_b0alpha0_lambda_n0rho0}"
EXECUTABLE=/home/xiang/Downloads/bytesteady/bytesteady/audio_classification/borgy/${MODEL}.sh

borgy submit --image zhangxiangxiao/bytesteady \
      --volume /home/xiang:/home/xiang \
      --volume /mnt/scratch:/mnt/scratch \
      --cpu ${CPU} --mem ${MEMORY} --restartable \
      -- bash $EXECUTABLE "${@:4}" -train_thread_size ${CPU};
