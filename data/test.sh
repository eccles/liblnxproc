#!/bin/sh
#----------------------------------------------------------------------------
srcdir=$1
#echo "srcdir=${srcdir}"
top_srcdir=$2
#echo "top_srcdir=${top_srcdir}"
if [ -s "../buildname" ]
then
    BUILDNAME=`cat ../buildname`
else
    BUILDNAME=build
fi
#echo "BUILDNAME=${BUILDNAME}"
#echo "PWD=${PWD}"
#----------------------------------------------------------------------------
# lots of fiddling here as gcov does not seems to handle VPATH library builds 
# well. The source files are hardlinked into the build directory with the automake
# prefix added to correspond to the gcno files.
#
process_source_files() {
    PFX=$1
    DIR=$2
    DST=../${DIR}
    SRC=${PWD}/${top_srcdir}/${DIR}
    for i in `ls ${SRC}/*.c`
    do
        FILE=`basename $i`
        ln ${SRC}/${FILE} ${DST}/${PFX}-${FILE}
    done
    if which lcov >/dev/null 2>&1
    then
        cd ..
        lcov --directory ${DIR} --capture --output-file ${DIR}.info
        genhtml -o coverage-${DIR} ${DIR}.info
    else
        cd ${DST}
        gcov -b *.c > ${PWD}/../coverage_${DIR}.log
    fi
    cd -
}
#----------------------------------------------------------------------------
LD_LIBRARY_PATH=../lib
export LD_LIBRARY_PATH
ulimit -c unlimited
if [ "${BUILDNAME}" = "build" ]
then
    BIN=../bin/topiary
    if [ -s ${BIN} ]
    then
        echo "Compare output to standard set"
        export TOPIARY_TESTROOT=${srcdir}
#        ${BIN} test > testoutput
        ${BIN} test | grep -v '^Timestamp' | sort > testoutput
        diff testoutput ${srcdir}/testdata > testoutput.diff
    else
        echo "Unable to compare output to standard set - ${BIN} unavailable"
    fi
elif [ "${BUILDNAME}" = "valgrind" ]
then
    if which valgrind >/dev/null 2>&1
    then
        BIN=../bin/test-topiary
        if [ -s ${BIN} ]
        then
            echo "Test memory leakage"
            VALOPTS="--track-fds=yes --leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
            valgrind -v ${VALOPTS} ${BIN} >/dev/null
        else
            echo "Unable to test memory leakage - ${BIN} unavailable"
        fi
    else
        echo "Unable to test memory leakage - valgrind unavailable"
    fi
elif [ "${BUILDNAME}" = "gcov" ]
then
    if which gcov >/dev/null 2>&1
    then
        BIN=../bin/test-topiary
        if [ -s ${BIN} ]
        then
            echo "Test coverage"
            ${BIN} >/dev/null
            process_source_files libtopiary_la lib
            process_source_files test_topiary bin
        else
            echo "Unable to test coverage - ${BIN} unavailable"
        fi
    else
        echo "Unable to test coverage - gcov unavailable"
    fi
fi
