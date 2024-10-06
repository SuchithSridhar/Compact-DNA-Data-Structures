# move_structure
Table representation of runs-compressed BWT supporting backwards stepping.

# Example
### Download and Compile

```console
git clone https://github.com/drnatebrown/move_structure.git
cd move_structure

make
```

To compile using debug flags:
```console
make clean
make debug
```

### Build
The data folder holds two example files, ``dna.txt`` and ``dna.txt.bwt``

Builds the data structure on the given ``[FILENAME].bwt``, outputting ``[FILENAME].LF_move``
```console
./build ./data/dna.txt
```

### Inversion
Pass ``[FILENAME]`` (assuming build has been run) and ``[INVERT_FILENAME]``

```console
./invert ./data/dna.txt ./data/dna.txt.rev
```

To verify correctness:
```console
rev dna.txt.rev | diff - dna.txt
```

# Config
The ``include/LF_table.hpp`` file contains define statements to adjust the size of the data structure:
```console
#define LENGTH_BYTES 4
#define POINTER_BYTES 2
#define OFFSET_BYTES 2
```

Note that overflow errors may occur, so these should be adjusted for use case.
