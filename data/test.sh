#!/bin/sh
#----------------------------------------------------------------------------
srcdir=$1
echo "srcdir=${srcdir}"
top_srcdir=$2
echo "top_srcdir=${top_srcdir}"
echo "PWD=${PWD}"
HAVE_GCOV=$3
echo "HAVE_GCOV=${HAVE_GCOV}"
HAVE_LCOV=$4
echo "HAVE_LCOV=${HAVE_LCOV}"
HAVE_VALGRIND=$5
echo "HAVE_VALGRIND=${HAVE_VALGRIND}"
CFLAGS=$6
echo "CFLAGS=${CFLAGS}"
echo "${CFLAGS}" | grep "\-DDEBUG=1"
DEBUG=$?
echo "DEBUG=${DEBUG}"
LD_LIBRARY_PATH=../lib
export LD_LIBRARY_PATH
#-------------------------------------------------------------------------------
# lots of fiddling here as gcov does not seem to handle autotools library builds
# well. The source files are hardlinked into the build directory with the 
# automake prefix added to correspond to the gcno files.
#
LISTFILES=
process_source_files() {
    LISTFILES=
    local PFX=$1
    local DIR=$2
    local DST=../${DIR}
    local SRC=${PWD}/${top_srcdir}/${DIR}
    for i in `ls ${SRC}/*.c`
    do
        local FILE=`basename $i`
        echo $FILE | grep "${PFX}-" >/dev/null
        if [ $? -ne 0 ]
        then
            if [ ! -s ${DST}/${PFX}-${FILE} ]
            then
                #echo ln ${SRC}/${FILE} ${DST}/${PFX}-${FILE}
                ln ${SRC}/${FILE} ${DST}/${PFX}-${FILE}
            fi
            LISTFILES="$LISTFILES ${PFX}-${FILE}"
        fi
    done
}
cleanup_source_files() {
    cd ../$1
    #echo rm -rf ${LISTFILES}
    rm -rf ${LISTFILES}
    cd -
    LISTFILES=
}
#-------------------------------------------------------------------------------
test_standard_data() {
    local BIN=../bin/topiary
    if [ -s ${BIN} ]
    then
        echo "Compare output to standard set (${BIN})"
        TOPIARY_TESTROOT=${srcdir}
        export TOPIARY_TESTROOT
        libtool --mode=execute ${BIN} test | grep -v '^Timestamp' | sort > testoutput
        if [ $? -ne 0 ]
        then
            echo "${BIN} failure"
            exit 1
        fi
        diff testoutput ${srcdir}/testdata > testoutput.diff
    else
        echo "Unable to compare output to standard set - ${BIN} unavailable"
    fi
}
#-------------------------------------------------------------------------------
test_standard_test() {
    local BIN=../bin/test-topiary
    if [ -s ${BIN} ]
    then
        echo "Test standard (${BIN})"
        TOPIARY_TESTROOT=
        export TOPIARY_TESTROOT
        libtool --mode=execute ${BIN} >/dev/null
        if [ $? -ne 0 ]
        then
            echo "${BIN} failure"
            exit 1
        fi
    else
        echo "Unable to test standard set - ${BIN} unavailable"
    fi
}
#-------------------------------------------------------------------------------
#test_standard_data_test() {
#    local BIN=../bin/test-topiary
#    if [ -s ${BIN} ]
#    then
#        echo "Test standard data (${BIN} srcdir=${srcdir})"
#        TOPIARY_TESTROOT=${srcdir}
#        export TOPIARY_TESTROOT
#        libtool --mode=execute ${BIN} >/dev/null
#        if [ $? -ne 0 ]
#        then
#            echo "${BIN} failure"
#            exit 1
#        fi
#    else
#        echo "Unable to test standard data set - ${BIN} unavailable"
#    fi
#}
#-------------------------------------------------------------------------------
test_valgrind() {
    if which valgrind >/dev/null 2>&1
    then
        BIN=../bin/test-topiary
        if [ -s ${BIN} ]
        then
            echo "Test memory leakage (${BIN})"
            TOPIARY_TESTROOT=
            export TOPIARY_TESTROOT
            VALOPTS="--track-fds=yes --leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
            libtool --mode=execute valgrind -v ${VALOPTS} ${BIN} >/dev/null
            if [ $? -ne 0 ]
            then
                echo "${BIN} failure"
                exit 1
            fi
        else
            echo "Unable to test memory leakage - ${BIN} unavailable"
        fi
    else
        echo "Unable to test memory leakage - valgrind unavailable"
    fi
}
#-------------------------------------------------------------------------------
test_gcov() {
    if which gcov >/dev/null 2>&1
    then
        local BIN=../bin/test-topiary
        if [ -s ${BIN} ]
        then
            echo "Test coverage (${BIN})"
            TOPIARY_TESTROOT=
            export TOPIARY_TESTROOT
            VALOPTS="--track-fds=yes --leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
            libtool --mode=execute ${BIN} >/dev/null
            if [ $? -ne 0 ]
            then
                echo "${BIN} failure"
                exit 1
            fi

            process_source_files libtopiary_la lib
            cd ../lib
            gcov -b ${LISTFILES} > ${PWD}/../coverage_lib.log
            cd -
            cleanup_source_files lib

            process_source_files test_topiary bin
            cd ../bin
            gcov -b ${LISTFILES} > ${PWD}/../coverage_bin.log
            cd -
            cleanup_source_files bin

        else
            echo "Unable to test coverage - ${BIN} unavailable"
        fi
    else
        echo "Unable to test coverage - gcov unavailable"
    fi
}
#-------------------------------------------------------------------------------
test_lcov() {
    if which lcov >/dev/null 2>&1
    then
        local BIN=../bin/test-topiary
        if [ -s ${BIN} ]
        then
            echo "Test coverage (${BIN})"
            TOPIARY_TESTROOT=
            export TOPIARY_TESTROOT
            VALOPTS="--track-fds=yes --leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes"
            libtool --mode=execute ${BIN} >/dev/null
            if [ $? -ne 0 ]
            then
                echo "${BIN} failure"
                exit 1
            fi

            process_source_files libtopiary_la lib
            cd ..
            lcov --directory lib --capture --output-file lib.info
            genhtml -o coverage-lib lib.info
            cd -
            cleanup_source_files lib

            process_source_files test_topiary bin
            cd ..
            lcov --directory bin --capture --output-file bin.info
            genhtml -o coverage-bin bin.info
            cd -
            cleanup_source_files bin

        else
            echo "Unable to test coverage - ${BIN} unavailable"
        fi
    else
        echo "Unable to test coverage - lcov unavailable"
    fi
}
#-------------------------------------------------------------------------------
ulimit -c unlimited
state=0
if [ ${HAVE_LCOV} -eq 1 ]
then
    state=1
    test_lcov
fi
if [ ${HAVE_GCOV} -eq 1 ]
then
    state=1
    test_gcov
fi
if [ ${HAVE_VALGRIND} -eq 1 ]
then
    test_valgrind
fi

if [ ${state} -eq 0 ]
then
    if [ ${DEBUG} -eq 0 ]
    then
        test_standard_test
#        test_standard_data_test
    else
        test_standard_data
    fi
fi
