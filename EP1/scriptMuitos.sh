#!/bin/bash

for i in 1 2 3
do
    for j in $(seq 1 30)
    do
        ./ep1sim $i muitos.txt muitos-$i-$j.txt d
    done
done
