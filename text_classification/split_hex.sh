#!/bin/bash

# Split lines in a hex data file
# Copyright 2019 Xiang Zhang
#
# Usage: bash split_lines.sh [lines] [input] [output_prefix]
#
# Note: .hex postfix will be automatically added.

set -x;
set -e;

split -d -a 1 --additional-suffix=.hex -l $1 $2 $3;
