#!/bin/bash

# script

# for i in `seq 1 30`
# do
#     filename='input'
#     ./ep3 < $filename
#     rm fs
# done
echo ---------------------- cp 1MB, rm 1MB ----------------------
filename='input'
for i in `seq 1 3`
do
    echo "$i";
    ./ep3 < $filename | grep "microsegundos (cp)"
    rm fs
done

#(./ep3 < input ) >> out
#./ep3 < input | grep "microsegundos"
