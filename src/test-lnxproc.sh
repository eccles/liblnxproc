#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
if [ $# -eq 0 ]
then
    echo "Must give at least one argument"
    exit 1
fi
OPT=$1
shift
VALOPTS="--leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
if [ "${OPT}" = "leak" ]
then
    valgrind -v ${VALOPTS} ./test-lnxproc $*
elif [ "${OPT}" = "leakdbg" ]
then
    valgrind -v ${VALOPTS} ./test-lnxproc-dbg $*
elif [ "${OPT}" = "ddd" ]
then
    ddd test-lnxproc $*
elif [ "${OPT}" = "ddddbg" ]
then
    ddd test-lnxproc-dbg $*
elif [ "${OPT}" = "dbg" ]
then
    ./test-lnxproc-dbg $*
elif [ "${OPT}" = "nodbg" ]
then
    ./test-lnxproc $*
fi
exit 0
