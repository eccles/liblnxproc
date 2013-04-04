#!/bin/bash
#
EXE=$( type splint )
if [ $? -ne 0 ]
then
	echo "splint is not accessible (maybe not installed)"
	exit 1
fi
if [ $# -lt 1 ]
then
	echo "You must specify C filename(s) as first argument"
	exit 1
fi
#
#echo ":$*:"
code=0
D=$( dirname $0 )
CNF=${D}/splint.conf
for i in $*
do
    splint -f ${CNF} -nof $i
    if [ $? -ne 0 ]
    then
        code=$?
    fi
done
exit $code
