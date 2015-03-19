/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
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

#ifndef DEVICE_INCLUDED
#define DEVICE_INCLUDED



/**
\file

\brief Container (Basic) class for all Dations and interrupt devices.

*/

namespace pearlrt {

   /**
   \brief This is a container class to create a defined interface
          for the compiler.

   PEARL specifies, that system- and problempart can be compiled separatly.
   but dations and interrupts are only specified in problempart.
   but at compiletime you have to say at problempart that device
    is "extern"-type.
   but the compiler don't know wether the datatype is a "dation" an
   "interrupt" or something.
   so they have to generalized to the abstract type "device" and later
   cast back to it's type.

   example :

     PEARL:
   \code
   SYSTEM;
   file: Disc("/", 5);
   PROBLEM;
   SPC file DATION INOUT ALL;
   \endcode


   C++:
   \code
   //  generated from system part
   static Disc _file("/",5);
   Device* __file = &_file;

   //  generated from problem part
   extern Device* __file;

   //   The Static-cast can be performed by means of the key word "Dation"
   //     and the key word "ALL"
   SystemDationNB* _file = static_cast<SystemDationNB*>(__file);
   \endcode

   */



   class Device {

   };


}

#endif
