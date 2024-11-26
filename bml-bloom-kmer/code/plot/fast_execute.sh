./../build/build_kmer_filter ../data/dataset.txt tmp/dataset_test.blm 5 100000 1

./../build/kmer_substrings ../data/single_pattern.txt tmp/dataset_test.blm 10 > tmp/substring_test.txt

./../build/move_mem_finder ../data/dataset1.mvt ../data/tesatad1.mvt tmp/substring_test.txt 10 > tmp/output_test.txt