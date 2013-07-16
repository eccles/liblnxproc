#!/bin/sh
make distclean
rm -rf build debug topiary*.tar.gz
find -type f -name '*~' -exec rm -f {} \; -print
find -type f -name 'x' -exec rm -f {} \; -print
find -type f -name 'y' -exec rm -f {} \; -print
find -type f -name 'z' -exec rm -f {} \; -print
find -type f -name 'testoutput*' -exec rm -f {} \; -print
find -type f -name '*.o' -exec rm -f {} \; -print
find -type f -name '*.a' -exec rm -f {} \; -print
find -type f -name '*.la' -exec rm -f {} \; -print
find -type f -name '*.lo' -exec rm -f {} \; -print
find -type f -name '*.so' -exec rm -f {} \; -print
find -type f -name 'core*' -exec rm -f {} \; -print
find -type f -name 'gmon.out' -exec rm -f {} \; -print
find -type d -name '.libs' -exec rm -rf {} \; -print
find -type d -name '.deps' -exec rm -rf {} \; -print
exit 0
