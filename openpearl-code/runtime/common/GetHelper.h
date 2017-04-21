#ifndef GETHELPER_H_INCLUDED
#define GETHELPER_H_INCLUDED
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

\brief input formating  helper object

This class provides several helper routines for Get-processing.
The detection of field separators is realized. In case a field separator is
encountered within the field width, the field is terminated and the field
separator is discarded.

The EndOfLine is expected in UNIX style as simple newline character.
The I/O-processing must translate other behavior to this pattern.

\author R. Mueller

*/

#include <stdint.h>
#include <stdio.h>
#include "Fixed.h"
#include "Fixed63.h"
#include "Clock.h"
#include "Source.h"
#include "RefChar.h"


namespace pearlrt {
   /**
   \addtogroup io_common_helper
   @{
   */
  
   /**
     \brief Helper class providing utility functions for input format
     processing.

     The functions operate on a source object and manage
     the input data processing of the input field of a given length.
     Field separators are detected.
   */
   class GetHelper {
   private:
      int width;
      Source * source;
      int delimiter;

   public:
      /**
      calculate 10**exp 

      \param exp the exponent. 
      \returns pow(10,exp)
      */
      static double pow10(int exp);

      /**
      create a Helper object

      \param w the input field width
      \param s the source object
      */
      GetHelper(const Fixed<31> w,
                Source * s);

      /**
      return remaining characters in input field
      \returns the number of left caharacters in the input field
      */
      int getRemainingWidth();

      /**
      skip spaces in the input field

      \returns 0, if a non space character was found<br>
              -1, if end of field was reached<br>
              -2, if end of record
      */
      int skipSpaces();

      /**
      discard remaining input field
      */
      void discardRemaining();

      /**
      read integral value with maximum digits

      The reading stops if the value is larger than the MAX_INT

      \param x pointer to int value for the data element
      \param digits number of digits allowed

      \returns number of processed digits (> 0 on normal execution)
      */
      int readInteger(int * x, const int digits);


      /**
      read integral value with exact number of digits

      The reading stops if the value is larger than the MAX_INT

      \param x pointer to int value for the data element
      \param digits number of digits required

      \returns number of processed digits (>0 on normal execution)
      */
      int readFixedInteger(int * x, const int digits);

      /**
      read mantissa of a floating point number with maximum digits

      The function expects nether sign nor leading spaces.
      The function stops at the fisr not fitting character.

      \param x pointer to int value for the data element
      \param digits number of digits allowed
      \param decimals number of decimal. This element is applied,
             if the input field does not contain a decimal point

      \returns number of processed digits (> 0 on normal execution)
      */
      int readMantissa(double * x, const int digits, const int decimals);


      /**
       reads a  value for the seconds part ##.####

       The point value may not contain a sign ('-' or '+').
       If there is a decimal point, the parameter d is obsolete.

       If there is no decimal point, the last d digits are interpreted
       as 'post decimal point'. Obviously there must be at least d digits
       in the field for this case. If not -1 is returned

       If no decimal point is detected, it is ok, if no prepoint digits
       are present.

       Any non digit character, or second '.' terminated to scanning.
       If no legal numerical value was found an error is returned.

       \param x pointer to the double value for the read data
       \param w input field width
       \param d number of decimals
       \returns number of treated digits (>0 on normal execution)
      */
      int readSeconds(double * x, const int w, const int d);

      /**
      read given string pattern

      \param s C-string containing the exact pattern of characters
               to be found in the input field
      \returns 0, if the pattern was found<br>
               -1, else
      */
      int readString(const char* s);

      /**
      field delimiter codes.
      Several codes may be ored together.

      The default value is 0 -- the exact field width must be reached.
      */
      enum Delimiters {
         /** 
         treat two (2) space characters as field delimiter.

         This is useful for keyboard input.
         */
         DoubleSpace = 1,

         /**
         accept end of line ('\\n') as input delimiter
         */
         EndOfLine = 2,

         /**
         accept comma (',') as input delimiter
         */
         Comma = 4,
         /**
         accept end of file  as input delimiter
         */
         EndOfFile = 8
      };

      /**
       set delimiter combinations

      \param del the neu set of delimiters as OR-combination of the
        enum Delimiters elements
      */
      void setDelimiters(int del);


      /** read next input character
       \return next char, or <br>
       -1, if a field delimiter was found<br>
       or -2, if no more characters were found (and EOF is no delim)

       \note Only the delimiting NewLine character will be left in
             the input stream
      */
      int readChar();


      /**
      read character variable

      reading stops at EndOfLine or after w characters.

      \param rc RefChar elements which gives access to the char variable

      \throws CharacterFormatSignal if length <= 0
      */
      void readCharacterByA(RefCharacter * rc);

      /**
       read hex value

       \param value pointer to the returns value
       \param nbrOfBitsToSample length of the target bit string
       \throws BValueSignal  if illegal characters found
      */
      void readB4(uint64_t * value, const int nbrOfBitsToSample);

      /**
       read bin value in format B1, B2 or B3

       \param value pointer to the returns value
       \param nbrOfBitsToSample length of the target bit string
       \param base is 1(B1), 2(B2) or 3(B3)-Format
       \throws BValueSignal  if illegal characters found
      */
      void readB123(uint64_t * value, const int nbrOfBitsToSample,
                    const int base);

      /**
      read fixed variable

      reading stops at EndOfLine or after w characters.

      \param value pointer to int64_t for the input value
      \param d number of decimals to simulate if no decimal point
               is in the field

      \throws FixedValueSignal if illegal characters were found
      \throws FixedFormatSignal if parameters were wrong
      */
      void readFixedByF(Fixed63 * value, int d);

      /**
      read float variable

      reading stops at EndOfLine or after w characters.

      \param value pointer to the input value
      \param d number of decimals to simulate if no decimal point
               is in the field

      \throws FixedValueSignal if illegal characters were found
      \throws FixedFormatSignal if parameters were wrong
      */
      void readFloatByF(Float<53> * value, int d);

      /**
      read float variable

      reading stops at EndOfLine or after w characters.

      \param value pointer to the input value

      \throws FixedValueSignal if illegal characters were found
      \throws FixedFormatSignal if parameters were wrong
      */
      void readFloatByE(Float<53> * value);
   };
   /** @} */
}
#endif

