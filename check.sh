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
INDTYPES="-T LNXPROC_ARRAY_T -T LNXPROC_ARRAY_ITERATE_FUNC -T LNXPROC_BASE_T -T LNXPROC_BASE_METHOD -T LNXPROC_ERROR_T -T LNXPROC_ERROR_CALLBACK"

AST=$( type astyle )
if [ $? -ne 0 ]
then
	echo "astyle is not accessible (maybe not installed)"
    exit 1
fi
ASTOPTS="--style=1tbs --break-blocks=all --break-closing-brackets --align-pointer=middle"
ASTTYPES="-T LNXPROC_ARRAY_T -T LNXPROC_ARRAY_ITERATE_FUNC -T LNXPROC_BASE_T -T LNXPROC_BASE_METHOD -T LNXPROC_ERROR_T -T LNXPROC_ERROR_CALLBACK"

if [ $# -lt 1 ]
then
	echo "You must specify C filename(s) as first argument"
	exit 1
fi
#
#echo ":$*:"
code=0
D=$( dirname $0 )
#CNF=${D}/splint.conf
for i in $*
do
#    splint -f ${CNF} -nof $i
#    if [ $? -ne 0 ]
#    then
#        code=$?
#    fi
    indent ${INDOPTS} ${INDTYPES} $i
#    astyle ${ASTOPTS} $i
    if [ $? -ne 0 ]
    then
        code=$?
    fi
done
exit $code
