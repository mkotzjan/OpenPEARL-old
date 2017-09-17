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

\brief Data type CHARACTER

This class provides the operations of the data type char.

*/

#include "Character.h"

namespace pearlrt {
   Character<1>  toChar(int x) {
      Character<1> c;
      c.data[0] = x;
      return c;
   }

   int characterCompare(const char*s1, int l1, const char* s2, int l2) {
      int i;
      int len = l1 < l2 ? l1 : l2;

      for (i = 0; i < len; i++) {
         if (*s1 != *s2) {
            break;
         }

         s1 ++;
         s2 ++;
      }

      if (i < len) {
         return *s1 - *s2;
      } else {
         return 0;
      }

      if (len < l2) {
         // string 1 was shorter -- let's see if there is a
         // difference in the remaining part of string2 in
         // comparison with space ' '

         for (i = len; i < l2 && *s2 == ' '; i++) {
            s2 ++;
         }

         if (i == l2) {
            return 0;  // no difference found
         } else {
            return *s2 - ' ';
         }
      }

      if (len < l1) {
         // string 2 was shorter -- let's see if there is a
         // difference in the remaining part of string1 in
         // comparison with space ' '

         for (i = len; i < l1 && *s1 == ' '; i++) {
            s2 ++;
         }

         if (i == l1) {
            return 0;  // no difference found
         } else {
            return *s1 - ' ';
         }
      }
   }


   void characterFillSpaces(char * dest, int len) {
      for (int i = 0; i < len; i++) {
         *dest ++ = ' ';
      }
   }

   void characterSafeCopy(char*dest,
                          char* source, int len) {
      if (source < dest) {
         // the source string has a smaller adress - the two strings
         // may overlap in case of copy charSlices
         //    source    XXXXXX
         //    dest        XXXXXX
         // --> let's start at the end
         dest += len - 1;
         source += len - 1;

         for (int i = 0; i < len; i++) {
            *dest-- = *source--;
         }
      } else {
         //    source        XXXXXX
         //    dest        XXXXXX
         // --> let's start at the beginning - overlaps would no
         //     make problems
         for (int i = 0; i < len; i++) {
            *dest++ = *source++;
         }
      }
   }

}
