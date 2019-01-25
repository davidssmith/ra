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

test_reshape () {
  FILE=$1
  ORIG_DIMS=$(./radims $FILE)
  ELBYTE=$(./rahead $FILE | grep elbyte | awk '{print $2}')
  SIZE=$(./rahead $FILE | grep size | awk '{print $2}')
  NUMEL=$(( $SIZE / $ELBYTE ))
  printf "RESHAPE $FILE from $ORIG_DIMS to $NUMEL x 1 ... "
  cp $FILE tmp.ra
  ./rareshape tmp.ra $NUMEL
  ./rareshape tmp.ra $ORIG_DIMS
  ./radiff $FILE tmp.ra
  checkresult
  rm -f tmp.ra
}

test_reshape ../data/randc32.ra

# clean up
rm -f tmp.ra tmp2.ra
