#!/bin/sh

checkresult () {
  if [ $? -eq 0 ]; then
      echo pass
  else
      echo "fail (code: $?)"
  fi
}

code2type () {
    case $1 in
        1) TYPE=i ;;
        2) TYPE=u ;;
        3) TYPE=f ;;
        4) TYPE=c ;;
    esac
    BYTE=$(( 8 * $2))
}        
        

dotest () {
  FILE=$1
  code2type $2 $3
  ORIG_ELTYPE=$(./rahead $FILE | grep eltype | awk '{print $2}')
  ORIG_ELBYTE=$(./rahead $FILE | grep elbyte | awk '{print $2}')
  ORIG_TYPE=$(./rahead $FILE | grep "^type" | awk '{print $2}')
  printf "CONVERT $FILE from $ORIG_TYPE to $TYPE$BYTE ... "
  ./raconvert $2 $3 $FILE tmp.ra
  ./raconvert $ORIG_ELTYPE $ORIG_ELBYTE tmp.ra tmp2.ra
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

dotest ../data/randi8.ra 1 1
dotest ../data/randi8.ra 1 2
dotest ../data/randi8.ra 1 4
dotest ../data/randi8.ra 1 8
dotest ../data/randi8.ra 3 2
dotest ../data/randi32.ra 1 4
dotest ../data/randi32.ra 1 8
dotest ../data/randu8.ra 2 1
dotest ../data/randu8.ra 2 2
dotest ../data/randu8.ra 2 4
dotest ../data/randu8.ra 2 8
dotest ../data/randu8.ra 3 2
dotest ../data/randu8.ra 3 4
dotest ../data/randu8.ra 3 8
dotest ../data/randu32.ra 2 8
dotest ../data/randf16.ra 3 4
dotest ../data/randf16.ra 3 8
#dotest ../data/randf32.ra 3 2
dotest ../data/randf32.ra 3 4
dotest ../data/randf32.ra 3 8
dotest ../data/randc32.ra 4 8
dotest ../data/randc32.ra 4 16


# clean up
rm -f tmp.ra tmp2.ra
