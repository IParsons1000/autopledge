#!/bin/bash
#
# (c)2025 Ira Parsons
# autopledge test suite - test-elf core
#

TESTFILE=$1
TESTLOG=$2
CURRDIR=$3
TOP=$4
SUCCESS=0

echo "[INFO]  test-elf: starting" >> ${TESTLOG}

LD_LIBRARY_PATH=$TOP $CURRDIR/test-elf

case $? in
  0)
    SUCCESS = 1
    ;;
  1)
    echo "[ERROR] test-elf: elf_load failed (returned NULL)" >> ${TESTLOG} 
    ;;
  2)
    echo "[ERROR] test-elf: elf_get_dynsym failed (return NULL)" >> ${TESTLOG}
    ;;
esac

if [ $SUCCESS -eq 0 ] ; then
  echo "[INFO]  test-elf: failed" >> ${TESTLOG}
  echo "test-elf : F" >> ${TESTFILE}
else
  echo "[INFO]  test-elf: completed successfully" >> ${TESTLOG}
  echo "test-elf : S" >> ${TESTFILE}
fi
