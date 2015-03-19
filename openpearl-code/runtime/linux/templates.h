/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Rainer Mueller
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
\file

\brief Templates for math functions

Many functions must be supplied for different types.
These functions are realised as templates.
If a template is used with a wrong type confusing error messages will occur.
This is ok, since the compiler checks the validity of the types
*/

/**
abs

Deliver the absolute value.
Applicapable for Fixed(*), Float(*), Duration.

\tparam X the type of the current parameter

\param x the current value
\returns -x, if x <0; <br> x else
*/
template<class X> X abs(X& x) {
   if (x < X(0)) {
      return -x;
   } else {
      return x;
   }
}


/**
sign

Deliver the sign of the  value.
Applicapable for Fixed(*), Float(*), Duration.

\tparam X the type of the current parameter

\param x the current value
\returns -1, if x <0; <br> 0 if x == 0<br> +1, if x> 0
*/
template<class X> Fixed<31>  sign(X& x) {
   X null(0);
   Fixed<31> retval;

   if (x < null) {
      retval = -1;
   } else if ( x == null) {
      retval = 0;
   } else {
      retval = 1;
   }
   return retval; 
}


