CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -O3

HEADERS = $(wildcard include/*.hpp)

all: build invert

# Target to build the executable for build.cpp
build: src/build.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o build src/build.cpp

# Target to build the executable for invert.cpp
invert: src/invert.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o invert src/invert.cpp

.PHONY: debug
debug: CXXFLAGS += -g -O0
debug: all

# Clean up build files
clean:
	rm -f build invert