#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
valgrind -v --leak-check=full --show-reachable=yes --read-var-info=yes ./test-lnxproc
#ddd test-lnxproc
exit 0
