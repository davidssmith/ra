#!/bin/sh

(
for i in `seq 1 10`; do
	./timing
	./hdf5
done
) | sort
