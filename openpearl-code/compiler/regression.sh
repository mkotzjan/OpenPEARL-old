#!/usr/bin/env bash

cd ../testsuite

#############################################################################
#

function runtest {
    echo "test=$1  expected=$2"
}

TESTS=`cat testlist`

awk -f testsuite/testlist 'BEGIN { FS = "[ \t\n]+" } { print $1 }'

## create or empty output directory:
if [ -d "out" ]
then
	rm out/*
else
	mkdir out
fi

for i in $TESTS
do
	echo "========================================================================"
	echo "run test: $i"
	test=`basename $i`
	prl_c --output out/$test $i
done

exit 1
