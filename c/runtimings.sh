#!/bin/sh

./timing 10
./hdf5 10

for i in `seq 1 10`; do
	./pngtiming eight
done
for i in `seq 1 10`; do
	./pngtiming airplane
done

