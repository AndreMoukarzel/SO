#!/bin/bash

for k in poucos medio muitos
do
	for i in 1 2 3
	do
	    for j in $(seq 1 10)
	    do
	        ./ep1sim $i $k.txt $k-$i-$j.txt d
	    done
	done
done