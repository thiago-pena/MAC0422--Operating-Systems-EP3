#!/bin/bash

# script

make

for i in `seq 1 3`
do
    ./ep3
    echo "mount a1\n"
done
