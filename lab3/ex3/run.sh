#!/bin/bash

srun make

for f in `ls *.in`
do
    echo "Running $f"
    srun ./ex3 < $f
done
