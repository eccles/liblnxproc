gcc -DDEBUG=1 -E -I../src results.c > results.hash
egrep -v '^#' results.hash > results.trim
../check.sh results.trim 
cp results.c results-pristine.c
mv results.trim results.c
exit 0
