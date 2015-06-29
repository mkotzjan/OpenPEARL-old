#!/bin/bash

M4FILES=`ls *.prlm4`

for file in $M4FILES 
do
	output=`basename $file .prlm4`
	rm -f "$output.prl"
done

