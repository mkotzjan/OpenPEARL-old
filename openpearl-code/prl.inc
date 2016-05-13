# /bin/sh
# Name: prl
#echo "called with :" $@
#
# preset variables

#----- read list of supported targets
#include "runtime/targets.inc"

#INTERFACE=/dev/ttyUSB0
#include "flashinterface.inc" 

if [ "${OpenPEARLTarget}" = "" ]
then
   PLATTFORM=linux
fi
COMPILEONLY=
RUN=
HELP=

while [ "$1" != "" ] ; do
    case $1 in
        -b) PLATTFORM=$2;  shift 2;;
        -i) INTERFACE=$2;  shift 2;;
        -c) COMPILEONLY=1;        shift;;
        -r) RUN=1;                shift;;
        -h) HELP=1;        shift;;
	 *) if [ "$INPUT" != "" ] ; then
               echo "illegal parameter " $1; 
               shift;
	       exit 1;
            fi;
            INPUT=$1; shift;;
     esac
done


#echo "HELP=" $HELP 
#echo PLATFORM = $PLATFORM
#echo COMPLEONLY = $COMPLEONLY
#echo INPUT = $INPUT

if [ -n "$HELP" ]
then 
   echo "Usage $0 [parameters] file"
   echo "   Parameters: " 
   echo "    -c compile only until C++ source" 
   echo "    -b <plattform> build for plattform (defaults to linux)" 
   echo "        or OpenPEARLTarget environment variable" 
   echo "        known plattforms: $plattforms"
   echo "    -r run application on MCU" 
   echo "    -i <interface> download interface for MCU (default /dev/ttyUSB0)" 
   echo "    -h this page" 
   echo "   Example: "
   echo "     prl x.prl -c   : compile only until C++ source"
   echo "     prl x.prl      : compile and link"
   echo "     prl x.cc       : compile previously created C++ source and link"
   echo "     prl -r x       : run application "
   exit 1
fi

#echo Plattform: $PLATTFORM
#echo plattforms: $plattforms
found=0
for p in $plattforms; do
   if [ "$p" = "$PLATTFORM" ]
   then
      found=1;
   fi
done
if [ $found -eq 0 ]
then
   echo "unsupported target plattform: "$PLATTFORM;
   exit 1;
fi

inp_ext=${INPUT##*.}
fn=${INPUT%.*}
  
#echo ext: $inp_ext
#echo fn: $fn 

if [ "$RUN" == "1" ] ; then
   if [ "$fn" != "$ext" ] ; then
#include "runtime/run_bin.inc"
      if [ $rc -ne 0 ]; then
          echo "*** program terminated with error ($rc) ***"
      fi
   else
      echo "specify executable file to perform run "
      exit 1
   fi
else
if [ "$inp_ext" == "prl" ] ; then
   prlcc=1;
else if [ "$inp_ext" != "cc" ] ; then
   echo "cannot treat extension " $inp_ext;
   exit 1;
   fi;
fi

#echo "prlcc=" $prlcc
if [ "$prlcc" == "1" ] ; then 
#   echo "PRL-> C++"
#include "compiler/prl_cc.inc"
rc=$?
if [ $rc -ne 0 ]
then
 echo "terminate due to errors in stage 1"
 exit $rc
fi

else  if [ "$COMPILEONLY" == "1" ] ; then
   echo " C++ input conflicts with option -c"
   exit 1
   fi
fi
if [ "$COMPILEONLY" != "1" ] ; then
  #   echo "C++->BIN"
#include "runtime/cc_bin.inc"
  if [ $rc -ne 0 ]
  then
     echo "compilation stage 2 terminated with errors"
     echo " --> check PEARL source code for problems"
     exit $rc
  fi
fi
fi

