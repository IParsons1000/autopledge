#!/bin/bash
#
# (c)2025 Ira Parsons
# autopledge test suite - poc-llama core
# 
# CVE-2025-49847
#

TESTFILE=$1
TESTLOG=$2
CURRDIR=$3
TOP=$4
SUCCESS=0

echo "[INFO]  poc-llama: starting" >> ${TESTLOG}

SHELLCODE="abc 1 2 3"

cd $(CURRDIR)/llama.cpp
build/bin/llama-server -hf ggml-org/gemma-3-1b-it-GGUF &
curl -H "Content-Type: application/json" -d '{ "content": "$SHELLCODE" }' http://127.0.0.1:8080/tokenize
killall llama-server

case $? in
  0)
    if [ "$UNPROT" -ne 1 ] ; then
      echo "[ERROR] poc-llama: shellcode is not functional without protection" >> ${TESTLOG}
    else
      SUCCESS=1
    fi
    ;;
  1)
    echo "[ERROR] poc-llama: shellcode was successful (returned 1)" >> ${TESTLOG} 
    ;;
  *)
    if [ "$UNPROT" -ne 1 ] ; then
      echo "[ERROR] poc-llama: shellcode is not functional without protection" >> ${TESTLOG}
    else
      echo "[ERROR] poc-llama: unexpected return value ($?)" >> ${TESTLOG}
    fi
    ;;
esac

if [ "$SUCCESS" -eq 0 ] ; then
  echo "[INFO]  poc-llama: failed" >> ${TESTLOG}
  echo "poc-llama : F" >> ${TESTFILE}
else
  echo "[INFO]  poc-llama: completed successfully" >> ${TESTLOG}
  echo "poc-llama : S" >> ${TESTFILE}
fi
