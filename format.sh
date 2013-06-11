#!/bin/bash
#
#SPL=$( type splint )
#if [ $? -ne 0 ]
#then
#	echo "splint is not accessible (maybe not installed)"
#	exit 1
#fi
IND=$( type indent )
if [ $? -ne 0 ]
then
	echo "indent is not accessible (maybe not installed)"
    exit 1
fi
INDOPTS="-kr -nut -l80 -nce -bad -bap -sob -psl"
INDTYPES=" \
-T ALLOCATE_T \
-T _LNXPROC_ARRAY_T \
-T _LNXPROC_ARRAY_ITERATE_FUNC \
-T _LNXPROC_BASE_T \
-T _LNXPROC_BASE_DATA_T \
-T _LNXPROC_BASE_TYPE_T \
-T _LNXPROC_BASE_METHOD \
-T LNXPROC_INTERFACE_METHOD \
-T _LNXPROC_LIMITS_T \
-T _LNXPROC_LIMITS_ROW_T \
-T _LNXPROC_METHOD \
-T LNXPROC_MODULE_T \
-T _LNXPROC_MODULE_ROW_T \
-T LNXPROC_MODULE_TYPE_T \
-T LNXPROC_OPT_T \
-T LNXPROC_PRINT_T \
-T _LNXPROC_RESULTS_T \
-T _LNXPROC_RESULTS_ITERATE_FUNC \
-T _LNXPROC_RESULTS_TABLE_T \
-T _LNXPROC_RESULTS_TABLE_VALUE_T \
-T _LNXPROC_RESULTS_TABLE_VALUETYPE_T \
-T _LNXPROC_VECTOR_T \
-T _LNXPROC_VECTOR_ITERATE_FUNC \
-T RELEASE_METHOD \
"
#AST=$( type astyle )
#if [ $? -ne 0 ]
#then
#	echo "astyle is not accessible (maybe not installed)"
#    exit 1
#fi
#ASTOPTS="--style=1tbs --break-blocks=all --break-closing-brackets --align-pointer=middle"
#ASTTYPES="-T LNXPROC_ARRAY_T -T LNXPROC_ARRAY_ITERATE_FUNC -T LNXPROC_BASE_T -T LNXPROC_BASE_METHOD -T LNXPROC_ERROR_T -T LNXPROC_ERROR_CALLBACK"

#if [ $# -lt 1 ]
#then
#	echo "You must specify C filename(s) as first argument"
#	exit 1
#fi
#
#echo ":$*:"
code=0
#CNF=${D}/splint.conf
DIRS="src include/lnxproc python test"
for i in ${DIRS}
do
#    splint -f ${CNF} -nof $i
#    if [ $? -ne 0 ]
#    then
#        code=$?
#    fi
    if [ -d $i ]
    then
        for j in $i/*.[ch]
        do
            echo "Indent $j ..."
            indent ${INDOPTS} ${INDTYPES} $j
#           astyle ${ASTOPTS} $i
            if [ $? -ne 0 ]
            then
                code=$?
            fi
        done
    else
        echo "Indent $i ..."
        indent ${INDOPTS} ${INDTYPES} $i
#       astyle ${ASTOPTS} $i
        if [ $? -ne 0 ]
        then
            code=$?
        fi
    fi
done
exit $code
