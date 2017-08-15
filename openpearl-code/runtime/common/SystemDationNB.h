/*
 [A "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014-2017 Rainer Mueller
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

#ifndef SYSTEMDATIONNB_INCLUDED
#define SYSTEMDATIONNB_INCLUDED

#include "SystemDation.h"
#include "Fixed.h"

/**
\file

\brief dation interface for non-BASIC systemdations (ALPHIC and TYPE)

*/

namespace pearlrt {

   /**
   \file

   \brief systemdation interface for non-BASIC dations
   */

   /**
     Defines the interfaces to communicate with a non-BASIC (ALPHIC,TYPE)
     dation.
     This type of systemdation contains the basic transfer operations
     for the device. It informs the UserDation abouts its capabilities
     <ul>
     <li>CAN : supports delete
     <li>NEW : supports file creation
     <li>...
     </ul>

     The API is defined generic (similar to the UNIX API), with
     dationOpen, dationClose, dationRead, dationWrite, dationUnGetChar
   */
   class SystemDationNB: public SystemDation {
   public:
      /**
      Open the device/file.

      To support multiple files on a system dation, the openDation must be
      realized as factory. The capacity is defined by the concrete system
      dation (e.g. 20 for files, or 1 for a serial line).

      \param idfValue as C-string containing the value of the IDF-field
      \param openParam a bit map containing the required operation

      \returns reference to the new object

      \throws * may throw exceptions in case of problems in execution
      */
      virtual SystemDationNB* dationOpen(const char * idfValue,
                                         int openParam) = 0;

      /**
      return one character back to the input stream.

      Only 1 character may be returned in sequence. More than 1 call in serie
      will store the latest character discarding the previous returned
      characters.

      \param c the character with should be returned to the input stream
      */
      virtual void dationUnGetChar(const char c) = 0;

      /**
      set newline translation mode

      The system dation is responsible to perform newline translations in
      a plattform specific manner, if the user dation is in ALPHIC mode.
      In case of usage in "Typed" mode (READ/WRITE) no translation must
      be performed. The class UserDationNB knowns this difference and 
      invokes this method accordingly.
     
      \param doNewLineTranslation must be true to enable the translation
      */
      virtual void translateNewLine(bool doNewLineTranslation) = 0; 

      /**
      set absolut position on dation/file

      This method must be overwritten by systemdations which are SEEKABLE

      The method applies only on DIRECT or FORBACK dations.
      FORWARD dations will write 0 bytes or discard input on read.

      \param p the target position of the read/write pointer (counted in bytes)
      \param dationParam specified the dation type (DIRECT,FORWARD,..)
      \throws may throw different exceptions - not defined yet
      */
      virtual void dationSeek(const Fixed<31> & p, const int dationParam) ;

      /**
      set position on dation/file to end of file
      and return the absolute position  if the dation is SEEKABLE

      \param dationParam specified the dation type (DIRECT,FORWARD,..)
      \throws may throw different exceptions - not defined yet
      */      
      virtual Fixed<31> dationEof();

   };
}
#endif
