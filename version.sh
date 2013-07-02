#!/bin/sh
#
# from Calcote - Autotools - Section 'Library vesioning is Interface Versioning'
#
current=0
revision=0
age=0

show_vers() {
    echo "Version $current:$revision:$age"
}
show_vers
echo -n "Has the source code changed at all [y/n]:"
read ans
if [ "$ans" = "y" ]
then
    revision=`expr $revison + 1`
elif [ "$ans" = "n" ]
then
    :
else
    echo "Illegal option '$ans'"
    exit 1
fi

show_vers
echo -n "Have any exported functions or data been added,removed or changed [y/n]:"
read ans
if [ "$ans" = "y" ]
then
    current=`expr $current + 1`
    revision=0
elif [ "$ans" = "n" ]
then
    :
else
    echo "Illegal option '$ans'"
    exit 1
fi

show_vers
echo -n "Have any exported functions or data been added [y/n]:"
read ans
if [ "$ans" = "y" ]
then
    age=`expr $age + 1`
elif [ "$ans" = "n" ]
then
    :
else
    echo "Illegal option '$ans'"
    exit 1
fi

show_vers
echo -n "Have any exported functions or data been removed [y/n]:"
read ans
if [ "$ans" = "y" ]
then
    age=0
elif [ "$ans" = "n" ]
then
    :
else
    echo "Illegal option '$ans'"
    exit 1
fi

show_vers
exit 0
