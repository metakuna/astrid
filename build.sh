#!/bin/bash

# deletes the current build and creates a new one in the build directory
# this is only needed when there has been a change to a CMakeLists.txt file
echo "deleting build directory"
rm -r build
mkdir build
cd build
cmake ..
make
cd ..

