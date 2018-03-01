/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

#ifndef LPC17XXUSB_INCLUDED
#define LPC17XXUSB_INCLUDED

#include "USB.h"

extern "C" {
   /** type of callback method to complete the device enumeration
    *  the callback must verify the device type and setup the required pipes
    */
   typedef void (*CompleteEnumeration_t)(const uint8_t corenum,
                                         size_t configDecriptorSize,
                                         void * decriptoreData);

   /** type of callback method to mark the current device to be disconnected
    */
   typedef void (*Disconnect_t)(void);

   /**
    * type of the callback method of the usb poll task
    */
   typedef void (*PollDevice_t)(void);

   /**
    * register the current device to the usb stack
    *
    * pass references of the callback methods
    */
   int registerUsbDevice(PollDevice_t pd,
                         CompleteEnumeration_t ce,
                         Disconnect_t d);

};

#endif

