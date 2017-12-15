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

#ifndef NUMBEROFBYTES_INCLUDE
#define NUMBEROFBYTES_INCLUDE
namespace pearlrt {
   /**
     \brief required number of bytes for a fixed variable

     helper template to calculate the number of required bytes to store
     the requested number of bits

     The number is adujusted to be the next possible power of 2.
     This is needed to use the native integral data types of c++

   */
   template< unsigned int len > class NumberOfBytes {
   public:
      /**
         calculation of the required number of bytes for a FIXED
         variable according to the given number of bits (without
         sign)
      */
      enum NumberOfBytesLen {
         NumberOfBytesN0 = (0 < len && len <= 8) ? 1 : 0,
         NumberOfBytesN1 = (8 < len && len <= 16) ? 2 : 0,
         NumberOfBytesN2 = (16 < len && len <= 32) ? 4 : 0,
         NumberOfBytesN3 = (32 < len && len <= 64) ? 8 : 0,
      };
      /**
         Number of bytes for a fixed variable
      */
      enum NumberOfBytesNbr {resultFixed = NumberOfBytesN0 + NumberOfBytesN1 +
                              NumberOfBytesN2 + NumberOfBytesN3};

      /**
         calculation of the required number of bytes for BitStrings
      */
      enum NumberOfBytesB {
         NumberOfBytesB0 = (0 < len && len <= 8) ? 1 : 0,
         NumberOfBytesB1 = (8 < len && len <= 16) ? 2 : 0,
         NumberOfBytesB2 = (16 < len && len <= 32) ? 4 : 0,
         NumberOfBytesB3 = (32 < len && len <= 64) ? 8 : 0,
      };
      /**
         result value of the calculation
      */
      enum R {resultBitString = NumberOfBytesB0 + NumberOfBytesB1 +
                                NumberOfBytesB2 + NumberOfBytesB3};
   };
}
#endif

