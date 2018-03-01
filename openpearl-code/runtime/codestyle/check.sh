#!/bin/bash

#/*
# [A "BSD license"]
# Copyright (c) 2012-2016 Rainer Mueller
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#*/

   BASEDIR=$(dirname $0)

   rm -f $1.new $1.diff
   if [ -e $1 ]
   then
      astyle --options=$BASEDIR/astyle.opt <$1 >$1.new
# some versions of astyle add an emptly line at the end of the file
# sed removes theempty line at the end of the new file
      sed -i '${/^$/d;}' $1.new
      diff --ifdef=NEWFORMAT $1 $1.new >$1.diff
      d=$?
      if [ "$d" != "0" ]
      then
         echo "*** file " $1 "seems to be not well formatted"
         echo " ** check "$1.diff " for version NEWFORMAT"
         echo " ** in case you agree with the new format copy "$1.new " to " $1
         exit 1
      fi

      awk -f $BASEDIR/longlines.awk $1
      long=$?
      if [ "$long" != "0" ] 
      then
         echo "*** long lines found --> truncate them manually"
         exit 1
      fi
   fi
   exit 0
 
