# Convert from wav file to raw format of a certain frequency
# Copyright ServiceNow 2021
#
# Usage: bash convert_raw.sh [format] [frequency] [input_directory] [output_directory]

set -x;
set -e;

FORMAT=${1:-s16le};
FREQUENCY=${2:-65536};
INPUT_DIRECTORY=${3:-data/wav};
OUTPUT_DIRECTORY=${4:-data/raw_65536hz};

for input_file in ${INPUT_DIRECTORY}/*/*.wav; do
    dir=$(basename $(dirname ${input_file}));
    mkdir -p ${OUTPUT_DIRECTORY}/${dir};
    output_file=${OUTPUT_DIRECTORY}/${dir}/$(basename ${input_file});
    ffmpeg -hide_banner -loglevel warning -y -i ${input_file} -f ${FORMAT} -ar ${FREQUENCY} -ac 1 -acodec pcm_s16le ${output_file:0:-3}raw;
done;
