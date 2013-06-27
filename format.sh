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
-T _TOPIARY_ARRAY_T \
-T _TOPIARY_ARRAY_ITERATE_FUNC \
-T _TOPIARY_BASE_T \
-T _TOPIARY_BASE_DATA_T \
-T _TOPIARY_BASE_TYPE_T \
-T _TOPIARY_BASE_METHOD \
-T TOPIARY_INTERFACE_METHOD \
-T _TOPIARY_LIMITS_T \
-T _TOPIARY_LIMITS_ROW_T \
-T _TOPIARY_METHOD \
-T TOPIARY_MODULE_T \
-T _TOPIARY_MODULE_ROW_T \
-T TOPIARY_MODULE_TYPE_T \
-T TOPIARY_OPT_T \
-T TOPIARY_PRINT_T \
-T _TOPIARY_RESULTS_T \
-T _TOPIARY_RESULTS_ITERATE_FUNC \
-T _TOPIARY_RESULTS_TABLE_T \
-T _TOPIARY_RESULTS_TABLE_VALUE_T \
-T _TOPIARY_RESULTS_TABLE_VALUETYPE_T \
-T _TOPIARY_VECTOR_T \
-T _TOPIARY_VECTOR_ITERATE_FUNC \
-T RELEASE_METHOD \
"
#AST=$( type astyle )
#if [ $? -ne 0 ]
#then
#	echo "astyle is not accessible (maybe not installed)"
#    exit 1
#fi
#ASTOPTS="--style=1tbs --break-blocks=all --break-closing-brackets --align-pointer=middle"
#ASTTYPES="-T TOPIARY_ARRAY_T -T TOPIARY_ARRAY_ITERATE_FUNC -T TOPIARY_BASE_T -T TOPIARY_BASE_METHOD -T TOPIARY_ERROR_T -T TOPIARY_ERROR_CALLBACK"

#if [ $# -lt 1 ]
#then
#	echo "You must specify C filename(s) as first argument"
#	exit 1
#fi
#
#echo ":$*:"
code=0
#CNF=${D}/splint.conf
DIRS="src include/topiary python test"
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
