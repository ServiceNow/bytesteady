#!/bin/bash
# Script to start borgy job
# Copyright 2019 Xiang Zhang
#
# Usage: bash borgy.sh [memory] [data] [model] ...

set -x;
set -e;

CPU="${1:-10}"
MEMORY="${2:-8}"
MODEL="${3:-gene_doublefnvnll_gram__size__dimension__a_b0alpha0_lambda_n0rho0split}"
EXECUTABLE=/home/xiang/Downloads/bytesteady/bytesteady/genome_tagging/borgy/${MODEL}.sh

borgy submit --image zhangxiangxiao/bytesteady \
      --volume /home/xiang:/home/xiang \
      --volume /mnt/scratch:/mnt/scratch \
      --volume /mnt/datasets:/mnt/datasets \
      --cpu ${CPU} --mem ${MEMORY} --restartable \
      -- bash $EXECUTABLE "${@:4}" -train_thread_size ${CPU};
