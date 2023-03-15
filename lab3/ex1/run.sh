#!/bin/bash

srun make

dirArray=("test_1a" "test_1b")

for dir in ${dirArray[@]}
do
    for f in `ls $dir`
    do
        tmp=${f: -3}
        if [ "$tmp" == ".in" ]
        then
            echo "Running $f"
            srun ./ex1 < $dir/$f
        fi
    done
done
