#!/bin/bash
echo "Building test"
g++ -I/home/will/Documents/proj/traj/src/cspice/include spk_test.cpp -L/home/will/Documents/proj/traj/src/cspice/lib -l:cspice.a
echo -e "Running test\n"
./a.out
echo -e "\nDeleting test executeable"
rm a.out
