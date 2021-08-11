"""
Utility functions for data

"""
import gzip
import numpy as np
import os

from collections import defaultdict
from itertools import product


# Constants for handling non-standard nucleotides
NON_STANDARD_MARKER = "*"
NON_STANDARD_NUCLEOTIDES = ["k", "m", "r", "y", "s", "w", "b", "v", "h", "d", "x", "n", "." , "-"]


def contig_flag_non_standard(s, replace_by=NON_STANDARD_MARKER):
    """
    Replaces all non-standard nucleotides in a sequence by a special marker

    Note: returned sequence will be lower case

    """
    s = s.lower()
    for n in NON_STANDARD_NUCLEOTIDES:
        s = s.replace(n, replace_by)
    return s


def filter_contigs_by_length(contigs, min=0, max=np.infty):
    """
    Deletes contigs that do not meet the length criteria

    """
    return [c for c in contigs if max >= len(c) >= min]


def find_all_genomes(path, extension=".fna.gz"):
    """
    List the genome files in a directory.

    """
    return [os.path.join(path, f) for f in os.listdir(path) if f.endswith(extension)]


def hash_string(sequence, k=3, bins=10, kmer_weights=None):
    """
    Computes the hashed representation of a string by:
    1. Extracting all the k-mers in the string
    2. Hashing each k-mer into "bins" possible values
    3. Counting the number of k-mers that end up in each bin

    Note: A weighted variant where each k-mer's presence is associated with a
          specific weight is also supported.

    """
    if sequence_contains_invalid(sequence):
        raise ValueError("The sequence contains invalid nucleotides.")

    if sequence_contains_non_standard(sequence):
        # We don't support hashing sequences with non-standard nucleotides,
        # since many non-standard nucleotides have an ambiguous matching to real
        # nucleotides. Considering them would likely induce noise. This is the
        # behavior used by many k-mer counting algorithms (e.g., ray surveyor).
        raise ValueError("The sequence contains non-standard nucleotides.")

    if kmer_weights is None:
        kmer_weights = defaultdict(lambda: 1.)

    n_kmers = 4**k
    if bins >= n_kmers:
        # We don't need a hashing function, there are enough bins
        alphabet = ["a", "c", "g", "t"]
        kmer_to_idx = {"".join(x): y for x, y in zip(list(product(*([alphabet] * k))), range(n_kmers))}
        def kmer_hasher(kmer):
            return kmer_to_idx[kmer]
    else:
        kmer_hasher = hash

    a = np.zeros(bins)
    n_valid_kmers = 0.
    for i in range(len(sequence)):
        kmer = sequence[i : i + k]
        # Skip k-mers that contain flagged non-standard nucleotides or that are too short
        if len(kmer) < k or \
           NON_STANDARD_MARKER in kmer:
            continue
        a[abs(kmer_hasher(kmer)) % bins] += kmer_weights[kmer]
        n_valid_kmers += 1
    return a, n_valid_kmers


def one_hot_dna(sequence):
    """
    One-hot encode a DNA nucleotide sequence.

    Convertions were obtained from http://www.boekhoff.info/dna-fasta-codes/

    """
    if sequence_contains_invalid(sequence):
        raise ValueError("The sequence contains invalid nucleotides.")

    if NON_STANDARD_MARKER in sequence:
        raise ValueError("The sequence to one-hot encode should not contain invalid nucleotide flags.")

    if len(sequence) == 0:
        return np.array([]).reshape(0, 4)

    convert = {"a": np.array([1, 0, 0, 0], dtype=np.float),
               "t": np.array([0, 1, 0, 0], dtype=np.float),
               "u": np.array([0, 1, 0, 0], dtype=np.float),
               "c": np.array([0, 0, 1, 0], dtype=np.float),
               "g": np.array([0, 0, 0, 1], dtype=np.float),
               "k": np.array([0, 1, 0, 1], dtype=np.float) / 2.,
               "m": np.array([1, 0, 1, 0], dtype=np.float) / 2.,
               "r": np.array([1, 0, 0, 1], dtype=np.float) / 2.,
               "y": np.array([0, 1, 1, 0], dtype=np.float) / 2.,
               "s": np.array([0, 0, 1, 1], dtype=np.float) / 2.,
               "w": np.array([1, 1, 0, 0], dtype=np.float) / 2.,
               "b": np.array([0, 1, 1, 1], dtype=np.float) / 3.,
               "v": np.array([1, 0, 1, 1], dtype=np.float) / 3.,
               "h": np.array([1, 1, 1, 0], dtype=np.float) / 3.,
               "d": np.array([1, 1, 0, 1], dtype=np.float) / 3.,
               "x": np.array([1, 1, 1, 1], dtype=np.float) / 4.,
               "n": np.array([1, 1, 1, 1], dtype=np.float) / 4.,
               ".": np.array([0, 0, 0, 0], dtype=np.float),
               "-": np.array([0, 0, 0, 0], dtype=np.float)
              }
    return np.vstack((convert[base] for base in sequence.lower()))


def fasta_to_contigs(path, return_headers=False):
    """
    Reads a FASTA file and loads its contigs

    Note: sequences are returned in lower case

    """
    contigs = []
    headers = []

    def add_contig(header, seq):
        if len(seq) == 0:
            raise Exception("Attempted to add a contig with length 0. Not normal! Path is " + path)
        if sequence_contains_invalid(seq):
            raise Exception("Sequence contains invalid characters.")
        contigs.append(seq.lower())
        headers.append(header.lower())

    buffer = ""
    header = ""
    for l in gzip.open(path, "rt", encoding="utf-8"):
        if l.startswith(">"):
            # New contig starting

            # Save current sequence and flush buffer
            if len(buffer) > 0.:
                add_contig(header, buffer)
                buffer = ""

            # Read contig header
            header = l[1:].strip()

        else:
            # Accumulate DNA sequence
            buffer += l.strip()

    # Save final buffer
    if buffer is not None and buffer != "":
        add_contig(header, buffer)

    if return_headers:
        return contigs, headers
    else:
        return contigs


def reverse_complement(sequence):
    """
    Computes the reverse complement of a DNA sequence

    Note: This function also supports non-standard nucleotides. The conversion
          rules were taken from http://www.boekhoff.info/dna-fasta-codes/

    """
    if sequence_contains_invalid(sequence):
        raise ValueError("The sequence contains invalid nucleotides.")

    sequence = sequence.lower()
    complement = {'a': 't',
                  'c': 'g',
                  'g': 'c',
                  't': 'a',
                  'k': 'm',
                  'm': 'k',
                  'r': 'y',
                  'y': 'r',
                  's': 's',
                  'w': 'w',
                  'b': 'v',
                  'v': 'b',
                  'h': 'd',
                  'd': 'h',
                  'x': 'n',
                  'n': 'n',
                  '.': '.',
                  '-': '-',
                  NON_STANDARD_MARKER: NON_STANDARD_MARKER}
    return ''.join([complement[base] for base in sequence[::-1]])


def sequence_contains_non_standard(sequence):
    """
    Checks if a sequence contains non-standard nucleotides

    """
    return any([x in sequence.lower() for x in NON_STANDARD_NUCLEOTIDES])


def sequence_contains_invalid(sequence):
    """
    Checks if a sequence contains invalid nucleotides

    """
    valid = dict(zip(NON_STANDARD_NUCLEOTIDES, [True] * len(NON_STANDARD_NUCLEOTIDES)))
    valid.update({"a": True, "c": True, "g": True, "t": True, NON_STANDARD_MARKER: True})
    for x in sequence.lower():
        if x not in valid:
            return True
    return False