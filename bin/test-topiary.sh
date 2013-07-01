#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
if [ $# -lt 1 ]
then
    echo "Must give at least 1 arguments"
    exit 1
fi
ulimit -c unlimited
BIN=./test-topiary
OPT=$1
shift
#echo "$#"
#exit 0
VALOPTS="--track-fds=yes --leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
if [ "${OPT}" = "leak" ]
then
    valgrind -v ${VALOPTS} ${BIN} $*
elif [ "${OPT}" = "leakdbg" ]
then
    valgrind -v ${VALOPTS} ${BIN}-dbg $*
elif [ "${OPT}" = "ddd" ]
then
    ddd ${BIN}
elif [ "${OPT}" = "ddddbg" ]
then
    ddd ${BIN}-dbg
elif [ "${OPT}" = "dddprf" ]
then
    ddd ${BIN}-prf
elif [ "${OPT}" = "dbg" ]
then
    ${BIN}-dbg $*
elif [ "${OPT}" = "prf" ]
then
    ${BIN}-prf $*
elif [ "${OPT}" = "nodbg" ]
then
    ${BIN} $*
fi
exit 0
