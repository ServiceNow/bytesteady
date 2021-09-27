# Gene classification dataset

The data used to create the gene classification dataset used in the paper was extracted from the [PATRIC](https://www.patricbrc.org/) database.

## Download
You can download the data files used in the paper [here](https://zenodo.org).

## Regenerating
To re-generate the dataset, follow these steps:
1. `pip install -r requirements.txt`
2. `python download_all_genomes.py` (note: long and very large download)
3. Run the `gene_classification_dataset.ipynb` notebook to extract the gene sequences and their labels.

**Note:** The PATRIC database is frequently updated. It is very likely that the dataset obtained from running the above commands today will be different from the one used in the paper.
