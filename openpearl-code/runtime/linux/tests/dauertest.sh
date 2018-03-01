#! /bin/bash

r=0
l=0
while [ $r -eq 0 ]
do
   echo "loop count $l"
   ./lot_of_tasks
   r=$?
   l=$(( $l + 1 ))
done

