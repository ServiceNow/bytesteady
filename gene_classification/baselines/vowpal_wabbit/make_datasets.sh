#!/bin/bash
# Creates the Vowpal Wabbit baseline datasets
# Usage: ./make_datasets kmer_configs

KMER_CONFIGS=$1
CODE_PATH="/mnt/home/adrouin/dev/element_ai/bytesteady/baselines/vw"
OUTPUT_PATH="/mnt/datasets/public/research/antibiotic_resistance/gene_classification/vw"

while read config; do
    if [[ $config != "#"* ]];then
        ram_needed=$(echo $config|cut -d' ' -f2)
        config=$(echo $config|cut -d' ' -f1)
        echo "KMER: $config -- RAM: $ram_needed"
        # Docker command to create the datasets
        borgy submit \
                     --cpu 1 \
                     --gpu 0 \
                     --mem $ram_needed \
                     --restartable \
                     --image images.borgy.elementai.net/vowpalwabbit \
                     --volume /mnt/datasets/public:/mnt/datasets/public \
                     --mount-home \
                     --name "Vowpal Wabbit - Dataset Creation ($config, $ram_needed GB)" \
                     -- \
                     sh -c \
                     "cd $OUTPUT_PATH;
                     python $CODE_PATH/make_vw_dataset.py $config"
    fi
done < $KMER_CONFIGS

echo "Ork launch detected."