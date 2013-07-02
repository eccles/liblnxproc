#!/bin/sh
#
#" makes all versions and test them
BASE=`pwd`
DIRS="build debug"
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
    LOG=$d.log
    MYCFLAGS=`cat ../cflags.$d`
    echo "Process '$d' directory with CFLAGS '${MYCFLAGS}'"
    ../configure CFLAGS="${MYCFLAGS}" >>${LOG}
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
    make check >>${LOG}
    if [ $? -ne 0 ]
    then
        echo "Make check '$d' FAILED"
        continue
    else 
        echo "Make check '$d' SUCCESS"
    fi
    rm -f ${LOG}
done
exit 0
