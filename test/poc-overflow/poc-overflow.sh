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

setarch -R env -i LD_PRELOAD=bananas $CURRDIR/poc-overflow < $CURRDIR/shellcode.bin

UNPROT=$?

setarch -R env -i LD_PRELOAD=$AUTOPLEDGE $CURRDIR/poc-overflow < $CURRDIR/shellcode.bin

case $? in
  0)
    if [ "$UNPROT" -ne 1 ] ; then
      echo "[ERROR] poc-overflow: shellcode is not functional without protection" >> ${TESTLOG}
    else
      SUCCESS=1
    fi
    ;;
  1)
    echo "[ERROR] poc-overflow: shellcode was successful (returned 1)" >> ${TESTLOG} 
    ;;
  *)
    if [ "$UNPROT" -ne 1 ] ; then
      echo "[ERROR] poc-overflow: shellcode is not functional without protection" >> ${TESTLOG}
    else
      echo "[ERROR] poc-overflow: unexpected return value ($?)" >> ${TESTLOG}
    fi
    ;;
esac

if [ "$SUCCESS" -eq 0 ] ; then
  echo "[INFO]  poc-overflow: failed" >> ${TESTLOG}
  echo "poc-overflow : F" >> ${TESTFILE}
else
  echo "[INFO]  poc-overflow: completed successfully" >> ${TESTLOG}
  echo "poc-overflow : S" >> ${TESTFILE}
fi
