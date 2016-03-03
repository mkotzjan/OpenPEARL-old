#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED
/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

\brief The array descriptor and access routines

\author R. Mueller

Arrays are treated in PEARL in a special way. They may be passed 
anonymously to procedures and the procedure does only know
the number of dimensions. There is no limit for aray dimensions.

*/


#include <stdio.h>
#include <stdint.h>

#include "Fixed.h"
#include "Signals.h"
#include "Log.h"

#if (TARGET == 2 && CONFIG_LPC1768_CHECK_STACK_OVERFLOW==1)
  // disable stack checking for the template methods in the applicaton code
# define NOSTACKCHECK __attribute__((no_instrument_function))
#else
# define NOSTACKCHECK /* nothing */
#endif

// some preprocessor definitions
#define LIMITS(...) __VA_ARGS__


#define DCLARRAY(name,dimensions,limits) \
   pearlrt::ArrayDescriptor<dimensions> a_##name = { dimensions, limits }; \
   pearlrt::Array b_##name((pearlrt::ArrayDescriptor<0>*)&a_##name); \
   pearlrt::Array * name = &(b_##name);

namespace pearlrt {
template <int DIM>
   struct ArrayDescriptor {
      int dim;
      struct Limits {
         int low,high, size;
      } lim[DIM];
   };

   class Array {
     private: 
      ArrayDescriptor<0>* descriptor;
      Array(){};
     public:
      Array(ArrayDescriptor<0> * descr) : descriptor(descr) {}

      size_t offset(Fixed<31> i, ...);

      Fixed<31> upb(Fixed<31> x); 
      Fixed<31> lwb(Fixed<31> x); 
  
   };
   
}
# undef NOSTACKCHECK
#endif

