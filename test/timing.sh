#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
ulimit -c unlimited
valgrind -v --leak-check=yes --read-var-info=yes ./timing
#./timing
exit 0
