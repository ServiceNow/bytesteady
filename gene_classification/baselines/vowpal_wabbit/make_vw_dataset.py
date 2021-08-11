"""
Copyright 2021 ServiceNow

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Create a vowpal wabbit dataset for the gene classification task.

"""
import numpy as np
import os
import pandas as pd
import sys

sys.path.append("../data")

from collections import defaultdict
from data_utils import *
from functools import partial
from math import ceil
from tqdm import tqdm


# Path that contains the ".cleartext" data files
RAW_DATA_PATH = (
    "/mnt/datasets/public/research/antibiotic_resistance/gene_classification/"
)
RAW_DATA_EXT = "cleartext"
KMER_CONFIG = sys.argv[1]
OUT_EXT = "%s.vw" % KMER_CONFIG


def assign_kmer_idx():
    """
    kmer indices are assigned on the fly

    """

    def kmer_counter():
        for i in range(999999999999):
            yield i

    counter = kmer_counter()
    return defaultdict(lambda: next(counter))


def kmer_lengths():
    """
    Kmer lengths to consider

    Supports:
    1) Ranges: [start]r[stop]r[increment]
    2) Enumerations: [n1]e[n2]e...e[nN]
    3) Single values: [n1]

    """
    if "r" in KMER_CONFIG:
        # Range
        spt = KMER_CONFIG.split("r")
        for k in range(int(spt[0]), int(spt[1]) + 1, int(spt[2])):
            yield int(k)
    elif "e" in KMER_CONFIG:
        # Enumeration
        for k in KMER_CONFIG.split("e"):
            yield int(k)
    else:
        # Single value
        yield int(KMER_CONFIG)


def write_dataset(name, kmer_to_idx):
    """
    Convert clear text data file to vowpal wabbit format

    """
    dataset = pd.read_csv(
        os.path.join(RAW_DATA_PATH, "%s.%s" % (name, RAW_DATA_EXT)), sep="\t"
    )

    vw_dataset = open("%s.%s" % (name, OUT_EXT), "w")
    for i, (idx, row) in enumerate(tqdm(dataset.iterrows(), total=dataset.shape[0])):
        sequence = contig_flag_non_standard(row.sequence)
        kmer_idx = []
        for k in kmer_lengths():
            for i in range(len(sequence)):
                kmer = sequence[i : i + k]
                if len(kmer) < k or NON_STANDARD_MARKER in kmer:
                    continue
                kmer_idx.append(kmer_to_idx[kmer])
        kmer_idx, counts = np.unique(kmer_idx, return_counts=True)
        if len(kmer_idx) == 0:
            print("\n" * 3, sequence, "\n" * 3)
            exit()
        data_line = "%d | %s\n" % (
            row.label + 1,
            " ".join(["k%d:%d" % (k, c) for k, c in zip(kmer_idx, counts)]),
        )
        vw_dataset.write(data_line)


if __name__ == "__main__":
    kmer_to_idx = assign_kmer_idx()
    package_data = partial(write_dataset, kmer_to_idx=kmer_to_idx)
    package_data("test")
    package_data("valid")
    package_data("train")
    package_data("train_valid")