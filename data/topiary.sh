#!/bin/bash
LD_LIBRARY_PATH=../lib
export LD_LIBRARY_PATH
if [ $# -lt 2 ]
then
    echo "Must give at least 2 arguments"
    exit 1
fi
ulimit -c unlimited
BIN=../bin/${0%.sh}
if [ "${1}" = "test" ]
then
    FUNC="${1}"
    export TOPIARY_TESTROOT=`pwd`
elif [ "${1}" = "time" ]
then
    FUNC="${1}"
else
    echo "Illegal first argument"
    exit 1
fi
shift
OPT=$1
shift
#echo "$#"
#exit 0
VALOPTS="--track-fds=yes --leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
if [ "${OPT}" = "leak" ]
then
    valgrind -v ${VALOPTS} ${BIN} ${FUNC} $*
elif [ "${OPT}" = "leakdbg" ]
then
    valgrind -v ${VALOPTS} ${BIN}-dbg ${FUNC} $*
elif [ "${OPT}" = "ddd" ]
then
    ddd ${BIN} ${FUNC}
elif [ "${OPT}" = "ddddbg" ]
then
    ddd ${BIN}-dbg ${FUNC}
elif [ "${OPT}" = "dddprf" ]
then
    ddd ${BIN}-prf ${FUNC}
elif [ "${OPT}" = "dbg" ]
then
    ${BIN}-dbg ${FUNC} $*
elif [ "${OPT}" = "prf" ]
then
    ${BIN}-prf ${FUNC} $*
elif [ "${OPT}" = "nodbg" ]
then
    if [ "${FUNC}" = "test" -a $# -eq 0 ]
    then
        ${BIN} ${FUNC} $* &>testoutput.raw
        grep -v '^Timestamp' testoutput.raw | sort > testoutput
        rm testoutput.raw
        diff testoutput testdata
    else
        ${BIN} ${FUNC} $*
    fi
fi
exit 0
