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

//#include "chip.h"
//#include "FreeRTOSConfig.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h"

/*
this module implements _write and _read for the retarget mechanism.
The prototypes are already defined deeply in the gcc-compiler.
The corresponding include is  here.*/

extern "C" {
   int _write(int fd, char * ptr, int len);

   int _read(int fd, char * ptr, int len);

};

namespace pearlrt {
   /**
   provide some methods for simple uart device drivers on different
   plattforms.
   */
   class Retarget {
public:    
     /**
     retrieve the next output character from a pending printf 

     \param nextTransmitChar pointer to the location, where the next
        character to transmit shozuld be stored
     \returns true, if there is a character to transmit<br>
              false, else
     */ 
     static bool getNextStdOutChar(char * nextTransmitChar);

    /**
    add the given character to the input buffer, if there is an active
    scanf-statement

    \param ch, the current input character
    \returns true, if the character was add to scnaf-input buffer<br>
             false, if there is currently no input active
    */
    static bool addNextStdInChar(char ch);

   /** switch to interrupt mode

   during system startup, retarget uses the polling mode.
   The invocation of this method switches to interrupt mode.
   */
   static void useInterrupts(bool use=true);
};
}

