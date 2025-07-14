#!/bin/bash
#
# (c)2025 Ira Parsons
# autopledge test suite - poc-overflow core
#

TESTFILE=$1
TESTLOG=$2
CURRDIR=$3
TOP=$4
SUCCESS=0

echo "[INFO]  poc-overflow: starting" >> ${TESTLOG}

setarch -R env -i AUTOPLEDGE=$AUTOPLEDGE $CURRDIR/poc-overflow < $CURRDIR/shellcode.bin

case $? in
  0)
    SUCCESS=1
    ;;
  1)
    echo "[ERROR] poc-overflow: shellcode was successful" >> ${TESTLOG} 
    ;;
  *)
    echo "[ERROR] poc-overflow: unexpected return value ($?)" >> ${TESTLOG}
esac

if [ "$SUCCESS" -eq 0 ] ; then
  echo "[INFO]  poc-overflow: failed" >> ${TESTLOG}
  echo "poc-overflow : F" >> ${TESTFILE}
else
  echo "[INFO]  poc-overflow: completed successfully" >> ${TESTLOG}
  echo "poc-overflow : S" >> ${TESTFILE}
fi
