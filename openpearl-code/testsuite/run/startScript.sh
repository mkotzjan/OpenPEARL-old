#! /bin/bash

if [ -f $1 ] ; then
    ./$1 > testLog.txt
    rc=$?
    if [ $rc -ne 0 ] ; then
        printf "%-40s : *** fail *** \n" $1
    else
        printf "%-40s :     pass     \n" $1
    fi
else
    printf "%-40s : did not compile \n" $1
fi
#exit $rc
