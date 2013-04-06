#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
valgrind -v --leak-check=yes --read-var-info=yes ./test-lnxproc
exit 0
