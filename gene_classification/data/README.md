# Gene classification dataset

The data used to create the gene classification dataset used in the paper is extracted from the [PATRIC](https://www.patricbrc.org/) database.

The dataset used in the paper is available [here](https://zenodo.org).

To re-generate the dataset, follow these steps:
1. `pip install -r requirements.txt`
2. `python download_all_genomes.py` (note: long and very large download)
3. Run the `gene_classification_dataset.ipynb` notebook to extract the gene sequences and their labels.

Note: The PATRIC database is frequently updated. It is very likely that the dataset obtained from running the above commands today will be different from the one used in the paper.