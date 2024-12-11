# Assignment 5 - Compact Data Structure

This repository contains code to generate data structures for assignment 5.

### Dependencies
The following dependencies are required:
- `sdsl` (Succinct Data Structure Library)
- `cmake`
- `gcc` (C++17 is required to compile)

### Build Instructions
To build from source:

```bash
git clone https://github.com/mohsenzakeri/CDS-A5
cd CDS-A5
mkdir build
cd build
cmake ..
make
```

This will generate the build and load binaries.

### Usage

To use the binaries:

```
./build <bwt_file> <output.ri>   # Generates the data structure
./load <output.ri>               # Loads the data structure
```

