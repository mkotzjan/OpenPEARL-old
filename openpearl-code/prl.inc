# /bin/sh
# Name: prl
#echo "called with :" $@
#
# preset variables
INTERFACE=/dev/ttyUSB0
PLATTFORM=${OpenPEARLTarget};
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
   echo "    -b <plattform> build for plattform (defaults to LINUX)" 
   echo "        or OpenPEARLTarget environment variable" 
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

#if [ "$PLATTFORM" == "" ] ; then
#   if [ "${OpenPEARLTarget}" != "" ] ; then
#      PLATTFORM=${OpenPEARLTarget};
#   fi;
#fi;

#echo $PLATTFORM

inp_ext=${INPUT##*.}
fn=${INPUT%.*}
  
echo ext: $inp_ext
echo fn: $fn 

if [ "$RUN" == "1" ] ; then
   if [ "$fn" != "$ext" ] ; then
      echo "just run"
#include "runtime/run_bin.inc"
   else
      echo "target to run is required "
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

echo "prlcc=" $prlcc
if [ "$prlcc" == "1" ] ; then 
#   echo "PRL-> C++"
#include "compiler/prl_cc.inc"
else  if [ "$COMPILEONLY" == "1" ] ; then
   echo " C++ input conflicts with option -c"
   exit 1
   fi
fi
if [ "$COMPILEONLY" != "1" ] ; then
#   echo "C++->BIN"
#include "runtime/cc_bin.inc"
fi
fi

