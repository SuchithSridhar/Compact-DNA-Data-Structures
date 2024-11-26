Here is how to find MEMs using our code.

The first command n builds the bloom filter:

```bash
./build/build_kmer_filter data/dataset.txt tmp/dataset_test.blm 5 100000 1
```

- `data/dataset.txt`: The dataset file to build the bloom filter over.
- `tmp/dataset_test.blm`: The file to store the bloom filter.
- `5`: The k-mer length.
- `100000`: The number of bits allocated to the filter.
- `1`: The insertion parameter.


The second command applies the filter to identify patterns in the text based on the k-mers from the original dataset. It concurrently executes MEM finding (with move tables) on each valid substring:

```bash
./../build/kmer_mem_finder data/dataset1.mvt data/tesadat1.mvt tmp/dataset_test.blm data/single_pattern.txt 10
```

- `data/dataset1.mvt`: The move table file.
- `data/tesadat1.mvt`: The reversed move table file.
- `tmp/dataset_test.blm`: The kmer filter file.
- `data/single_pattern.txt`: The pattern file to filter.
- `10`: The minimum mem length.

If you encounter any issues, please let us know (nr858090@dal.ca).





