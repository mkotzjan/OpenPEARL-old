#!/bin/bash

m4files=`ls *.prlm4`

for file in $m4files
do
	output=`basename $file .prlm4`
	m4 $file > "$output.prl"
done



TESTS=`ls *.prl`

nooftests=0
passed=0
failed=0

printf "%-40s :    prl->c++      prl->bin\n" "program"
for test in $TESTS
do
	nooftests=$(($nooftests + 1))

	prl_c $test >$test.out 2>&1
	rc=$?
     
        base=`basename $test .prl`
	prl_bin $base  1> /dev/null 2>&1
	rcbin=$?

	if [ $rc -eq 0 ]
	then
  		printf "%-40s :     PASSED    " "$test"
                if [ $rcbin -eq 0 ]
                then
		   passed=$(($passed + 1))
     		   printf "    PASSED\n" 
                else
     		   printf "*** FAILED ***\n" 
		   failed=$(($failed + 1))
                fi
	else
		failed=$(($failed + 1))
  		printf "%-40s : *** FAILED ***\n" "$test"
	fi
done

printf "Result:  no of tests/passed/failed: %d/%d/%d\n" "$nooftests" "$passed" "$failed"

