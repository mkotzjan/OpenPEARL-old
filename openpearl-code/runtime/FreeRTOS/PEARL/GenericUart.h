#ifndef GENERICUART_INCLUDED
#define GENERICUART_INCLUDED
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

#include "GenericUartDation.h"

namespace pearlrt {
  
   /**
   \brief Generic uart  low level support with Xon/Xoff protocol support

   Base class for all uart devices. Each real uart is derived from this class.
   This class is implemented as singleton, since it is also responsible
   to supply the support for printf via the retarget-mechanisme of the
   glibc. During the initialization of retarget the UART0-interface is 
   setup with default values. 

   In the very beginning in startup, we need a plattform specific
   uart object for the 
   retarget stuff. This is handled by a plattform specific
   implementation of SystemConsole.
   The creation of the SystemConsole is needed before the call of main.
   So we implement an singleton for the system console, which will instantiate
   the internal uart object.
   If a system dation is created by the PEARL application upon
   the same uart interface, the system dation
   object attaches the internal uart object.
   All operations of the system dation
   object is passed to the internal uart object.

   The internal uart object must be aware, if there is a system dation
   object, which may want to receive input data. Thus a system dation 
   must register a callback method to pass the output data if no 
   stdout or xon/xoff stuff is required.

   */
   class GenericUart {
   private:
      GenericUartDation * systemDation;
      bool isConsole;
      bool xonProtocol;
      bool doNewLineTranslation;

   protected:
      int status;  // same attribut in low level part

   public:
      /** 
      ctor of the native uart support

      Base class for all uart devices.
      Each real uart is derived from this class.
      The GenericUart provides XonXoff-protocol (see parameter xon)
      and support for retarget on stdin and stdout (see parameter _isConsole).

      \param xon XON/XOFF protocol selected (true = enable, false no protocol)
      \param _isConsole is true, if stdout operates on this uart
   */ 
   GenericUart(bool xon, bool _isConsole);
   
     enum GenericUartStatus {
      BREAK_INDICATOR = (1 << 0), // return code Break detected
      PARITY_ERROR = (1 << 1), // return code parity error
      FRAME_ERROR = (1 << 2),  // return code frame error
      RECEIVE_OVERRUN = (1 << 3), // return code receive overrun
      LF_MUST_SEND = (1 << 4), // internal marker for CR/LF transmission
      WRITE_IS_ACTIVE  = (1 << 5), // internal marker that a write is active
      READ_IS_ACTIVE  = (1 << 6), // internal marker that a read is active
      XOFF_MUST_SEND  = (1 << 7), // internal marker for xon/xoff protocol
      XOFF_SENT       = (1 << 8), // internal marker for xon/xoff protocol
      XOFF_RECEIVED   = (1 << 9), // internal marker for xon/xoff protocol
      RXCHAR_IS_BUFFERED = (1 << 10), // internal marker for xon/xoff protocol
      HAS_UNGETCHAR = (1 << 11), // an unGet-character is presen
      TX_INTERRUPT_PENDING = (1<<12),  //  tx char is in shift register 
      XON_MUST_SEND = (1<<13)     // xon must be sent 
     };

   /**
   register the dation object
   */
   void registerUartDation(GenericUartDation * u); 
   

   /**
    interrupt control of the UART device

    \param on  true enables the interrupts, false disables the interrupts
   */
   virtual void interruptEnable(bool on) = 0;

   /** 
   send an xon, if xoff was sent in before. This method is called, if a
   new GET statement is executed.
   */
   void sendXonIfNecessary();

   /**
    send next character from ether XOFF, from a StdOutJob, or 
    from the clients data buffer.
    If no data is available to send - nothing is done.
   */ 
   void sendNextChar();

   /**
   translate NewLine

   on input: translate CR into \n
   on output: translate \n ito CR+LF
   
   \param doNewLineTranslation enables/disables the translation
   */
   void translateNewLine(bool doNewLineTranslation);

   /**
   treat the received character as input or xon/xoff-flow control
  
   \param ch the received character
   */
   void doReceiveChar(char ch);

   /**
   Send the given character to the otput data register

   \param ch the character to send
   */
   virtual void sendChar(char ch) = 0;

   /**
   send the next character to the output data register if no
   transmit interrupt will occur automatically. This function
   restarts the output loop via the interrupt service routine.
   */
   virtual void triggerOutput() = 0;

   /**
   Send the given character in polling mode

   \param ch the character to send
   */
   virtual void sendCharPolling(char data) = 0;

   /**
   read the next character in polling mode

   \param ch the character to send
   */
   virtual char readCharPolling() = 0; 

   /**
   return the error status to the superior layer of the 
   uart driver
   */
   virtual int getErrorStatus() = 0;
};
}
#endif

