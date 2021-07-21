#!/bin/bash

# Shuffle lines in a text file
# Copyright 2021 ServiceNow
#
# Usage: bash shuffle_lines.sh [input] [output]

set -x;
set -e;

shuf $1 > $2;
