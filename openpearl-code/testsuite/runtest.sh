#!/bin/bash
PARAM=$1
shift
TESTS=$*

nooftests=0
passed=0
failed=0

printf "%-40s :    run\n" "program"
for test in $TESTS
do
   nooftests=$(($nooftests + 1))

  if [ -f $test ] ; then
     ./$test > $test.log
     rc=$?
     if [ $rc -ne 0 ] ; then
	    printf "%-40s :  *** fail ***\n" $test
 	    failed=$(($failed + 1))
     else
	    printf "%-40s :      pass    \n" $test
 	    passed=$(($passed + 1))
    fi
  else
    printf "%-40s :  did not compile\n" $test
    failed=$(($failed + 1))
  fi
done
	
printf "Result:  number of tests/passed/failed: %d/%d/%d\n" "$nooftests" "$passed" "$failed"

if [ $PARAM -eq 0 ] ; then
   exit 0
fi
if [ $PARAM -eq 1 ] ; then
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

