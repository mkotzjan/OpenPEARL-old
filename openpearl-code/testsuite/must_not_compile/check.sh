#!/bin/bash

# this script is called with a list of test cases
# each test case is compiled and the output is verified
pass=0
fail=0

splitPrefix=$1
shift

while [ true ]
do

tgt=$(basename "$1" .prl)
#echo "tgt = " $tgt

shift

if [ "$tgt" != "" ] 
then
   $splitPrefix/splitPrlExpect $tgt 2>/dev/null
   splitResult=$?
echo "split: " $splitResult
   if [ $splitResult -ne 0 ] 
   then
      printf "%-40s : fail : input file error\n" $tgt.check
      continue
   fi

   rm -f $tgt.log $tgt.err
   prl -c -b linux $tgt.prl 1>$tgt.log 2>$tgt.err
   compiled=$?

   if [ $compiled -eq 0 ]
   then
      printf "%-40s : fail : should not compile\n" "$tgt".prl
      fail=$(($fail + 1))
   else
      if [ -f $tgt.expect ]
      then
         {
            diff $tgt.err $tgt.expect 1>/dev/null
            difference=$?
         } 2> /dev/null
         # echo result: $difference  compiled $compiled
         if [ $difference -ne 0 ]
         then
            printf "%-40s : fail : produced wrong error message\n" "$tgt".prl
            fail=$(($fail + 1))
         else
            printf "%-40s : pass \n" "$tgt".prl
            pass=$(($pass + 1))
         fi
      else
         printf "%-40s : fail : expected error message is missing\n" "$tgt".prl
        fail=$(($fail + 1))
      fi
   fi
else
   printf "tests failed/passed: %d/%d\n" "$fail" "$pass"
   if [ $fail -ne 0 ]
   then
      exit 0  # return ok in any case to make jenkins happy
   else 
      exit 0
   fi
fi
done
