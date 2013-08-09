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
#------------------------------------------------------------------------------
log()  {
    echo "`date`:$* ..."
}
#------------------------------------------------------------------------------
for d in ${DIRS}
do
    cd $BASE
    rm -rf $d
    mkdir $d
    cd $d
    echo $d > buildname
    if [ ! -s ../options.$d ]
    then
        log "No options file ../options.$d"
        continue
    fi
    . ../options.$d
    log "Process '$d' directory with 'CFLAGS=\"${MYCFLAGS}\" ${MYCONFIGURE}'"
    ../configure CFLAGS="${MYCFLAGS}" ${MYCONFIGURE} >configure.log 2>&1
    if [ $? -ne 0 ]
    then
        log "Configure '$d' FAILED - see ${BUILDNAME}/configure.log for details"
        continue
    else 
        log "Configure '$d' SUCCESS"
    fi
    log "Make STARTED"
    make >make.log 2>&1
    if [ $? -ne 0 ]
    then
        log "Make '$d' FAILED - see ${BUILDNAME}/make.log for details"
        continue
    else 
        log "Make '$d' SUCCESS"
    fi
    log "Make html STARTED"
    make html >html.log 2>&1
    if [ $? -ne 0 ]
    then
        log "Make html '$d' FAILED - see ${BUILDNAME}/html.log for details"
        continue
    else 
        log "Make html '$d' SUCCESS"
    fi
    log "Make check STARTED"
    make check >check.log 2>&1
    if [ $? -ne 0 ]
    then
        log "Make check '$d' FAILED - see ${BUILDNAME}/check.log for details"
        continue
    else 
        log "Make check '$d' SUCCESS"
    fi
done
exit 0
