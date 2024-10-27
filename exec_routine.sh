#!/bin/bash

cd bml-bloom-kmer/code
make clean && make
./build/memFinder ../../suchi-move-table/data/travis_patterns.txt 7 11 10 ../../suchi-move-table/data/dataset.txt | tee output.txt
cd ../../suchi-move-table
make clean && make
./build/move_mem_finder data/dataset1.mvt data/tesatad1.mvt ../bml-bloom-kmer/code/output.txt | tee temp_output.txt
