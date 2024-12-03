This is an instruction file on how to run the mem finding algorithm in
this directory. `mem_finder.cpp` uses the RLFM index to perform mem finding.

You will need the BWT of the data and its reversed version. In other words,
you'll need two bwt files, which you can then use to build the `.ri` files which
is what is expected by the program.

First, once you have a data file, go to `../../../suchi-move-table/`. This
directory contains `bwt_generator` binary in the `bin/`. You should run this
program in the following way: `./bwt_generator both data.txt`. This will
generator `data.txt.bwt` and `data.txt.rev.bwt`. Now, come back to this build
directory and run the `./build/build` binary.

```sh
./bwt_generator both data.txt
# data.txt.bwt and data.txt.rev.bwt created
./build/build data.txt.bwt data.txt.ri
./build/build data.txt.rev.bwt data.txt.rev.ri
```

Then you want to run the actual mem finder on these indices:

```sh
./build/rlfm_mem_finder data.txt.ri data.txt.rev.ri patterns.txt 20
```
