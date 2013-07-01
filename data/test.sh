#!/bin/bash
#----------------------------------------------------------------------------
srcdir=$1
#----------------------------------------------------------------------------
LD_LIBRARY_PATH=../lib
export LD_LIBRARY_PATH
ulimit -c unlimited
BIN=../bin/topiary
export TOPIARY_TESTROOT=${srcdir}
#echo ${TOPIARY_TESTROOT}
${BIN} test >testoutput.raw 2>&1
grep -v '^Timestamp' testoutput.raw | sort > testoutput
rm testoutput.raw
diff testoutput ${srcdir}/testdata
