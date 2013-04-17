#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
if [ $# -eq 0 ]
then
    ./test-lnxproc
    exit 0
fi
VALOPTS="--leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
if [ "$1" = "leak" ]
then
    valgrind -v ${VALOPTS} ./test-lnxproc
elif [ "$1" = "leakdbg" ]
then
    valgrind -v ${VALOPTS} ./test-lnxproc-dbg
elif [ "$1" = "ddd" ]
then
    ddd test-lnxproc
elif [ "$1" = "ddddbg" ]
then
    ddd test-lnxproc-dbg
elif [ "$1" = "dbg" ]
then
    ./test-lnxproc-dbg
fi
exit 0
