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
#ifndef CAN2AMESSAGE_H_INCLUDED
#define CAN2AMESSAGE_H_INCLUDED
/**
\file

\brief common definition of CAN-bus device interface

*/

#include "SystemDationB.h"
#include "Fixed.h"
#include "BitString.h"
#include "Signals.h"


namespace pearlrt {

   /**

   \brief common definition of the interface of CAN-bus (2.0A) devices 
   for various system device implementations 

      This interface defines the structure of a base CAN (2.0A) message from/to
      the PEARL application on the PEARL side of the device driver.
      All PEARL drivers for CAN hardware must use this structure to
      enshure the independance between application and platform.
      An exception is the type of the data area. This must not exceed 64 bit -
      the type is irrelevant. 
   */

   class Can2AMessage {

   public:
     Fixed<11> identifier;    ///< the CAN identifier
     BitString<1> rtrRequest; ///< the Remote Transmission Request flag
     Fixed<4> dataLength;     ///< number of data to transfer
     BitString<8> data[8];    ///< the data storage for read and write

   };
}
#endif

