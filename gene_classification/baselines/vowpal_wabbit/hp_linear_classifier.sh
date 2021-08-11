#!/bin/bash

MAX_PASSES=100
N_CLASSES=6
DATA_DIR="/mnt/datasets/public/research/antibiotic_resistance/gene_classification/vw"
KMER_CONFIGS=$1

echo "Launching jobs..."
while read kmer_config
do
    if [[ $kmer_config != "#"* ]];then

        kmer_config=$(echo $kmer_config|cut -d' ' -f1)

        for l2 in 0 0.00000001 0.0000001 0.000001 0.00001 0.0001 0.001 0.01 0.1 1 10 100 1000
        do
            TRAIN_ONLY_FILE="train.$kmer_config.vw"
            TRAIN_VALID_FILE="train_valid.$kmer_config.vw"
            VALID_FILE="valid.$kmer_config.vw"
            TEST_FILE="test.$kmer_config.vw"


            WD="/mnt/datasets/public/research/antibiotic_resistance/gene_classification/vw/results/k_$kmer_config.l2_${l2}"
            echo "$l2 $kmer_config"
            borgy submit \
                         --cpu 1 \
                         --gpu 0 \
                         --mem 4 \
                         --restartable \
                         --image images.borgy.elementai.net/vowpalwabbit \
                         --volume /mnt/datasets/public:/mnt/datasets/public \
                         --mount-home \
                         --name "Vowpal Wabbit - Gene Classification $kmer_config $l2" \
                         -- \
                         sh -c \
                         "mkdir $WD;
                         cd $WD;
                         rm ./*;
                         vw --quiet --oaa $N_CLASSES --save_per_pass --passes $MAX_PASSES -k --cache_file ./train_valid.cache --l2 $l2 -f gene_classification.model $DATA_DIR/$TRAIN_VALID_FILE 2> /dev/null;
                         vw --quiet -i gene_classification.model -t $DATA_DIR/$TRAIN_ONLY_FILE -p train.predictions 2> /dev/null;
                         vw --quiet -i gene_classification.model -t $DATA_DIR/$VALID_FILE -p valid.predictions 2> /dev/null;
                         vw --quiet -i gene_classification.model -t $DATA_DIR/$TEST_FILE -p test.predictions 2> /dev/null"
        done
    fi
done < $KMER_CONFIGS

echo "Ork launch detected."
