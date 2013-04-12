#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
if [ $# -eq 0 ]
then
    ./test-lnxproc
    exit 0
fi
if [ "$1" = "leak" ]
then
    valgrind -v --leak-check=full --show-reachable=yes --read-var-info=yes ./test-lnxproc
elif [ "$1" = "ddd" ]
then
    ddd test-lnxproc-dbg
elif [ "$1" = "dbg" ]
then
    ./test-lnxproc-dbg
fi
exit 0
