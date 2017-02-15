#!/bin/sh

checkresult () {
  if [ $? -eq 0 ]; then
      echo pass
  else
      echo fail
      #exit
  fi
}

dotest () {
  FILE=$1
  ORIG_TYPE=$(./rahead $FILE | grep eltype | awk '{print $2}')
  ORIG_BYTE=$(./rahead $FILE | grep elbyte | awk '{print $2}')
  printf "CONVERT $FILE $2 $3 ... "
  ./raconvert $2 $3 $FILE tmp.ra
  ./raconvert $ORIG_TYPE $ORIG_BYTE tmp.ra tmp2.ra
  ./radiff $FILE tmp2.ra
  checkresult
}


printf "SQUASH int ... "
ORIG_TYPE=$(./rahead ../data/rand100.ra | grep eltype | awk '{print $2}')
ORIG_BYTE=$(./rahead ../data/rand100.ra | grep elbyte | awk '{print $2}')
./rasquash   ../data/rand100.ra tmp.ra
./raconvert  $ORIG_TYPE $ORIG_BYTE tmp.ra tmp2.ra
./radiff ../data/rand100.ra tmp2.ra
checkresult

printf "SQUASH float ... "
echo NOT IMPLEMENTED yet

dotest ../data/randi8.ra 1 2
dotest ../data/randi8.ra 1 4
dotest ../data/randi8.ra 1 8
dotest ../data/randi32.ra 1 8
dotest ../data/randu8.ra 2 2
dotest ../data/randu8.ra 2 4
dotest ../data/randu8.ra 2 8
dotest ../data/randu32.ra 2 8
dotest ../data/randf32.ra 3 8
dotest ../data/randc32.ra 4 8


# clean up
rm -f tmp.ra tmp2.ra
