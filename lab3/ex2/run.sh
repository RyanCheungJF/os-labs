#!/bin/bash

srun make

dirArray=("../ex1/test_1a" "../ex1/test_1b")

for dir in ${dirArray[@]}
do
    for f in `ls $dir`
    do
        tmp=${f: -3}
        if [ "$tmp" == ".in" ]
        then
            echo "Running $f"
            srun ./ex2 < $dir/$f
        fi
    done
done

echo "Running test.in"
srun ./ex2 < test.in
