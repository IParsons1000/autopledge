#!/bin/bash
#
# (c)2025 Ira Parsons
# autopledge test suite - test-recursive core
#

TESTFILE=$1
TESTLOG=$2
CURRDIR=$3
TOP=$4
SUCCESS=0

echo "[INFO]  test-recursive: starting" >> ${TESTLOG}

cd $CURRDIR
LD_LIBRARY_PATH=. LD_PRELOAD=$TOP/$AUTOPLEDGE ./test-recursive
OUT=$?
cd -

case $OUT in
  6)
    SUCCESS=1
    ;;
  *)
    echo "[ERROR] test-recursive: failed" >> ${TESTLOG}
    ;;
esac

if [ "$SUCCESS" -eq 0 ] ; then
  echo "[INFO]  test-recursive: failed" >> ${TESTLOG}
  echo "test-recursive : F" >> ${TESTFILE}
else
  echo "[INFO]  test-recursive: completed successfully" >> ${TESTLOG}
  echo "test-recursive : S" >> ${TESTFILE}
fi
