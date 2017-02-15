#!/bin/sh

./rasquash   ../data/rand100.ra tmp.ra
ORIG_TYPE=$(./rahead ../data/rand100.ra | grep eltype | awk '{print $2}')
ORIG_BYTE=$(./rahead ../data/rand100.ra | grep elbyte | awk '{print $2}')
./raconvert  $ORIG_TYPE $ORIG_BYTE tmp.ra tmp2.ra
./radiff ../data/rand100.ra tmp2.ra
echo diff: $?
