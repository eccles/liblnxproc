#!/bin/bash
#----------------------------------------------------------------------------
srcdir=$1
CFLAGS=$2
#----------------------------------------------------------------------------
LD_LIBRARY_PATH=../lib
export LD_LIBRARY_PATH
ulimit -c unlimited
BIN=../bin/topiary
#echo "cflags=${CFLAGS}"
echo "${CFLAGS}" | grep 'DDEBUG' >/dev/null 
if [ $? -eq 0 ]
then
    echo "No checking done as DEBUG is enabled"
    exit 0
fi
export TOPIARY_TESTROOT=${srcdir}
#echo ${TOPIARY_TESTROOT}
${BIN} test | grep -v '^Timestamp' | sort > testoutput
diff testoutput ${srcdir}/testdata > testoutput.diff

