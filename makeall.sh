#!/bin/sh
#
# makes all versions and test them
#
# build - vanilla build
# debug - vanilla build with DEBUG enabled
# valgrind - check memory leakage
# gcov - check test coverage analysis
#
BASE=`pwd`
DIRS="build debug valgrind gcov"
if [ $# -gt 0 ]
then
    DIRS=$1
fi
for d in ${DIRS}
do
    cd $BASE
    rm -rf $d
    mkdir $d
    cd $d
    echo $d > buildname
    LOG=$d.log
    if [ -s ../configure.$d ]
    then
        MYCFG=`cat ../configure.$d`
    else
        MYCFG=
    fi
    if [ -s ../cflags.$d ]
    then
        MYCFLAGS=`cat ../cflags.$d`
        echo "Process '$d' directory with 'CFLAGS=\"${MYCFLAGS}\" ${MYCFG}'"
        ../configure CFLAGS="${MYCFLAGS}" ${MYCFG} >>${LOG}
    else
        echo "Process '$d' directory with '${MYCFG}'"
        ../configure ${MYCFG} >>${LOG}
    fi
    if [ $? -ne 0 ]
    then
        echo "Configure '$d' FAILED"
        continue
    else 
        echo "Configure '$d' SUCCESS"
    fi
    make >>${LOG}
    if [ $? -ne 0 ]
    then
        echo "Make '$d' FAILED"
        continue
    else 
        echo "Make '$d' SUCCESS"
    fi
    make html >>${LOG}
    if [ $? -ne 0 ]
    then
        echo "Make html '$d' FAILED"
        continue
    else 
        echo "Make html '$d' SUCCESS"
    fi
    make check >>${LOG}
    if [ $? -ne 0 ]
    then
        echo "Make check '$d' FAILED"
        continue
    else 
        echo "Make check '$d' SUCCESS"
    fi
#    rm -f ${LOG}
done
exit 0
