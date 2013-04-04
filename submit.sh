#!/bin/bash
#
# major commit to new version
#
# specify version as first argument
#
if [ $# -lt 1 ]
then
    echo "Specify version as first argument"
    exit 1
fi
VERSION=$1
shift
OLDVERSION=$( cat version )
echo "VERSION = $VERSION"
echo "OLDVERSION = $OLDVERSION"
if [ "$VERSION" = "$OLDVERSION" ]
then
    echo "VERSION equals OLDVERSION"
    exit 1
fi

./genchanges.sh
echo "$VERSION" > version
git add .
if [ $? -ne 0 ]
then
    echo "Unable to add changed files"
    exit 1
fi
git commit -m"$VERSION : $*"
if [ $? -ne 0 ]
then
    echo "Unable to commit $VERSION"
    exit 1
fi
exit 0
