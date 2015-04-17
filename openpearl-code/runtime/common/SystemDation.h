/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
 Copyright (c) 2014-2014 Rainer Mueller
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

#ifndef SYSTEMDATION_INCLUDED
#define SYSTEMDATION_INCLUDED

#include "Dation.h"
#include "Character.h"
#include "Log.h"
#include <stdio.h>

/**
\file

\brief dation interface for systemdations (BASIC, ALPHIC and TYPE)

*/

namespace pearlrt{
   
/**
\file

\brief dation interface for systemdations (BASIC, ALPHIC and TYPE)

The dationOpen API uses char* instead of PEARL strings.

Each system device must report its capabilities.

*/

/**       
  Defines the interfaces to communicate with a non-BASIC (ALPHIC,TYPE)
  dation.
  This type of systemdation contains the basic transfer operations
  for the device. It informs the UserDation abouts its capabilities
  <ul>
  <li>CAN : supports delete
  <li>NEW : supports file creation
  <li>ANY : supports ANY
  <li>IDF : requires a file name
  <li>DIRECT : supports DIRECT
  <li>FORWARD : supports FORWARD
  <li>IN : transfer direction is INPUT
  <li>OUT : transfer direction is OUTPUT
  <li>INOUT : transfer direction is INPUT and/or OUTPUT
  </ul>

  The API is defined generic (similar to the UNIX API), with 
  dationOpen, define by SystemdationB and SystemDationNB
  dationClose, define by Systemdation
  dationRead, dationWrite  define by Dation
  dationSeek defined by SystemDationNB
  

  \note only type DIRECT and FORWARD are realized, yet
*/
class SystemDation: public Dation {
public:
    /**
    obtain the capabilities of the device

    This method must be implemented by each concrete SystemDation-class.
    It returns the support of the operations an user dations. The 
    return value is the ORed value of the enumeerations in Dation.h

    e.g.: <ul>
          <li> IN | IDF | PRM for a file on a CD 
          <li> INOUT | IDF | CAN | PRM | ANY | NEW | OLD for a file on disc
          </ul>
 
    \returns available commands of the device
    */
    virtual int capabilities() = 0;

    /**
    Close the device/file.

    The call of close will also return the handle to the factory pool.
   
    \param closeParam a bit map containing the required operation

    \throws * may throw exceptions in case of problems in execution
    */ 
    virtual void dationClose(int closeParam) = 0;

     /**
        Interface for the Close-interface, which is inherited
        from UserDation Basic-class

       \param p close parameters if given, else 0
       \param rst pointer to rst-variabla; required, if RST is set in p

        \note throws various exceptions if no RST-Variable is set

      */
    //void dationClose(int closeParam, Fixed<31>* rst);

#ifdef MOVEDTODATION
    /**
    Read from the device/file.
   
    \param destination target area to store the required number of bytes
    \param size number of bytes to read

    \throws * may throw exceptions in case of problems in execution
    */ 
    virtual void dationRead(void * destination, size_t size) = 0;
   
    /**
    Write to the device/file.
   
    \param destination source area to get the required number of bytes
    \param size number of bytes to write

    \throws * may throw exceptions in case of problems in execution
    */ 
    virtual void dationWrite(void * destination, size_t size) = 0;
   
    /**
    set absolut position on dation/file 

    The method applies only on DIRECT or FORBACK dations.
    FORWARD dations will write 0 bytes or discard input on read.
   
    \param p the target position of the read/write pointer (conted in bytes)
    \param dationParam specified the dation type (DIRECT,FORWARD,..)
    \throws may throw different exceptions - not defined yet
    */
    virtual void dationSeek(const Fixed<31> & p, const int dationParam) {
       Log::error("SystemDationNB: no seek implemented");
       throw theDationSeekNotRealizedSignal;
    }
#endif

};
}
#endif
