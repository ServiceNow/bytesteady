#!/bin/bash
# Script to start borgy job
# Copyright 2019 Xiang Zhang
#
# Usage: bash borgy.sh [memory] [data] [model] [threads] ...

set -x;
set -e;

MEMORY="${1:-8}";
THREADS="${4:-10}";

borgy submit --image zhangxiangxiao/bytesteady \
      --volume /home/xiang:/home/xiang \
      --volume /mnt/scratch:/mnt/scratch \
      --cpu $THREADS --mem $MEMORY --restartable \
      -- bash /home/xiang/Downloads/bytesteady/bytesteady/image_classification/train.sh "${@:2}";
