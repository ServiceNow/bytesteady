"""
Step 1: Download every single genome available in the PATRIC database.

Note:
    * We do not download genomes that do not meet some quality criteria
    * The species and genera are fetched from the NCBI taxonomy database
    * We download contigs and PATRIC annotations (PATtyFams)

"""
import numpy as np
import os
import pandas as pd

from collections import defaultdict
from joblib import delayed, Parallel
from math import ceil
from patric_tools.genomes import (
    download_genome_contigs,
    download_genome_features,
    download_genome_specialty_genes,
)
from patric_tools.genomes import get_latest_metadata
from tempfile import gettempdir
from xml.dom.minidom import parse


GENOME_CONTIGS_DIR = "./genomes/contigs"
GENOME_ANNOTATIONS_DIR = "./genomes/annotations"
GENOME_SPECIALTYGENES_DIR = "./genomes/specialty_genes"
GENOME_METADATA_SAVE_PATH = "./metadata.tsv"
DOWNLOAD_N_CONCURRENT = 30

# Genome quality criteria
MIN_GENOME_LENGTH = 1e5
MAX_N_CONTIGS = 200  # The number of contigs is generally inversely correlated with genome sequence quality

TMPDIR = "./tmp"


def download_genome(id):
    print("... id:", id)
    download_genome_contigs(id, GENOME_CONTIGS_DIR)
    download_genome_features(id, GENOME_ANNOTATIONS_DIR)
    download_genome_specialty_genes(id, GENOME_SPECIALTYGENES_DIR)


def get_genus_species_by_taxon_id(taxa):
    """
    Get the genus and species from the NCBI Taxonomy database based on a list
    of taxon IDs.

    Had to use curl because of problems with httplib on the EAI servers.
    This function is not thread safe since it uses a temp file with a fixed name.

    """
    chunk_size = 100
    n_chunks = int(ceil(1.0 * len(taxa) / chunk_size))
    taxon_info = defaultdict(lambda: defaultdict(lambda: None))
    for i in range(n_chunks):
        print(
            "...... querying NCBI taxonomy database (chunk {}/{}).".format(
                i + 1, n_chunks
            )
        )
        query = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=taxonomy&id={0!s}&retmod=xml".format(
            "+".join(str(x) for x in taxa[i * chunk_size : (i + 1) * chunk_size])
        )
        query_save_path = os.path.join(TMPDIR, "taxa.xml")
        os.system('curl "{}" > {}'.format(query, query_save_path))
        dom = parse(query_save_path)
        print("Done.")
        print("...... parsing XML")
        taxa_set = dom.getElementsByTagName("TaxaSet")[0]
        for taxon in [
            t
            for t in taxa_set.getElementsByTagName("Taxon")
            if t.parentNode == taxa_set
        ]:
            id = str(
                next(
                    element
                    for element in taxon.getElementsByTagName("TaxId")
                    if element.parentNode == taxon
                )
                .childNodes[0]
                .data
            )
            rank = [
                str(x.childNodes[0].data) for x in taxon.getElementsByTagName("Rank")
            ]
            scientific_name = [
                str(x.childNodes[0].data)
                for x in taxon.getElementsByTagName("ScientificName")
            ]
            info = dict(zip(rank, scientific_name))
            taxon_info[id]["species"] = info.get("species", None)
            taxon_info[id]["genus"] = info.get("genus", None)
    return taxon_info


print("Creating output directories...")
for dir in [GENOME_CONTIGS_DIR, GENOME_ANNOTATIONS_DIR, GENOME_SPECIALTYGENES_DIR]:
    os.makedirs(dir, exist_ok=True)


print("Downloading the metadata from PATRIC")
metadata = pd.read_table(
    get_latest_metadata(TMPDIR),
    usecols=[
        "genome_id",
        "genome_name",
        "genome_status",
        "genome_length",
        "contigs",
        "taxon_id",
    ],
    converters={
        "genome_id": str,
        "taxon_id": str,
        "genome_name": lambda x: x.replace("'", "").replace("[", "").replace("]", ""),
        "genome_status": lambda x: str(x).lower(),
    },
    index_col=False,
)
metadata = metadata.set_index(["genome_id"])
print("... Found {} genomes".format(metadata.shape[0]))

print("Discarding genomes of unknown type")
metadata = metadata.loc[
    (metadata.genome_status == "wgs") | (metadata.genome_status == "complete")
]
print("... {} genomes remaining".format(metadata.shape[0]))

print(
    "Discarding genomes with too many contigs (also discard those with no contig info)"
)
metadata = metadata.loc[(metadata.contigs.fillna(np.infty) <= MAX_N_CONTIGS)]
print("... {} genomes remaining".format(metadata.shape[0]))

print("Discarding genomes that are too small (unless it says the genome is complete)")
metadata = metadata.loc[
    (metadata.genome_length >= MIN_GENOME_LENGTH)
    | (metadata.genome_status == "complete")
]
print("... {} genomes remaining".format(metadata.shape[0]))

print("Generating genus and species names")
unique_taxa = metadata.taxon_id.unique()
taxon_info = get_genus_species_by_taxon_id(unique_taxa)
metadata["genus"] = metadata.taxon_id.apply(lambda x: taxon_info[x]["genus"])
metadata["species"] = metadata.taxon_id.apply(lambda x: taxon_info[x]["species"])
# Discard examples with missing genus or species
metadata = metadata.loc[
    ~pd.isnull(metadata.genus)
    & ~pd.isnull(metadata.species)
    & ~(metadata.genus == "candidatus")
    & ~(metadata.genus == "candidate")
    & ~(metadata.genus == "uncultured")
]
print("... There are {} genera".format(len(metadata.genus.unique())))
print("... There are {} species".format(len(metadata.species.unique())))
print("... {} genomes remaining".format(metadata.shape[0]))

print("Saving the metadata of the genomes to download")
metadata.to_csv(GENOME_METADATA_SAVE_PATH, sep="\t")

print("Downloading the genomes")
Parallel(n_jobs=DOWNLOAD_N_CONCURRENT)(
    delayed(download_genome)(id) for id in metadata.index.values
)

print("Done!")
