#!/bin/bash

test_files=$(ls $1 | grep .um -)

for file in $test_files ; do
    echo "Running test for" $file 
    valgrind ./um $1/$file >> output.txt
done