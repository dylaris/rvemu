#!/bin/sh

set -xe

echo "Testing cache..."
./nob cache
./rvsim --elf bin/nbench > cache.txt
sync

echo "Testing pure..."
./nob pure
./rvsim --elf bin/nbench > pure.txt
sync

echo "All tests completed."
