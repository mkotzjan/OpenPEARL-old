#!/bin/bash
PARAM=$1
shift
TESTS=$*

nooftests=0
passed=0
failed=0

printf "%-40s :    prl->c++      prl->bin\n" "program"
for test in $TESTS
do
	nooftests=$(($nooftests + 1))

	prl -c -b linux $test.prl >$test.out 2>&1
	rc=$?
     
        base=`basename $test .prl`
	prl -b linux $base.cc  1> /dev/null 2>&1
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

if [ $PARAM -eq 0 ]; then
   exit 0
fi
if [ $PARAM -eq 1 ]; then
   if [ $failed -ne 0 ]; then
      echo "at least 1 program did not compile"
      exit 1
   fi
   exit 0
fi
if [ $PARAM -eq 2 ]; then
   if [ $passed -ne 0 ]; then
      echo "at least 1 program did compile"
      exit 1
   fi
   exit 0
fi
echo "how did you reach this line?"
exit 3

