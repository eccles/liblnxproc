#!/bin/bash
LD_LIBRARY_PATH=../src
export LD_LIBRARY_PATH
if [ $# -eq 0 ]
then
    echo "Must give at least one argument"
    exit 1
fi
ulimit -c unlimited
BIN=${0%*.sh}
if [ "${0}" = "./testing.sh" ]
then
    export LNXPROC_TESTROOT=`pwd`
    rm -f testing
    ln timing testing
    rm -f testing-dbg
    ln timing-dbg testing-dbg
fi
#echo $LNXPROC_TESTROOT
#echo $BIN
#exit 0
OPT=$1
shift
VALOPTS="--leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
if [ "${OPT}" = "leak" ]
then
    valgrind -v ${VALOPTS} ${BIN} $*
elif [ "${OPT}" = "leakdbg" ]
then
    valgrind -v ${VALOPTS} ${BIN}-dbg $*
elif [ "${OPT}" = "ddd" ]
then
    ddd ${BIN} $*
elif [ "${OPT}" = "ddddbg" ]
then
    ddd ${BIN}-dbg $*
elif [ "${OPT}" = "dbg" ]
then
    ${BIN}-dbg $*
elif [ "${OPT}" = "nodbg" ]
then
    ${BIN} $*
fi
exit 0
