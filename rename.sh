#!/bin/bash
for i in *.[ch]
do
    sed -i "s/LIBLNXPROC/TOPIARY/g" $i
    sed -i "s/LNXPROC/TOPIARY/g" $i
    sed -i "s/liblnxproc/topiary/g" $i
    sed -i "s/lnxproc/topiary/g" $i
done
exit 0
