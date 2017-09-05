/*
 [The "BSD license"]
 Copyright (c) 2014 Rainer Mueller
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
#ifndef OCTOPUSDIGTALOUT_H_INCLUDED
#define OCTOPUSDIGTALOUT_H_INCLUDED
/**
\file

\brief Basic system device for Octopus Digital Output

*/

#include <octopus.h>
#include "Octopus.h"
#include "SystemDationB.h"
#include "Fixed.h"
#include "Signals.h"


namespace pearlrt {

   /**

   \brief Basic system device for Octopus Digital Output

      This device provides a simple digital output on the
      port bits of the octopus USB board.

   */

   class OctopusDigitalOut: public SystemDationB {

   private:
      const char port;
      const int start, width;
      Octopus * octo;

      void internalDationOpen();
      void internalDationClose();

   public:
      /**
      constructor to create the bit group and set the
      bits to output direction

      \note throws exception in case of init failure

      \param port identifier for port 'A', ...
      \param start bit number for the leftmost bit  (7..0)
      \param width number of bits (1..8)
      */
      OctopusDigitalOut(char port, int start, int width);

      /**
      destructor of the bit group

      This is supplied only for testing purpose.
      PEARL applications have static system dations, the dtor is called
      at systems end.
      */
      ~OctopusDigitalOut();

      /**
      Open the OctopusDigitalOut
     \param openParam open parameters if given
      \param idf pointer to IDF-value if given
      \param rstValue pointer to error variable if given 
      \tparam S the length of the file name; this is parameter is not
       required for this device
      \throws OpenFailedSignal, if  dation is not closed and rst is not given
      */
      template<size_t S>
      void dationOpen(int openParam=0,
                      Character<S>* idf=0,
                       Fixed<31>* rstValue=0) {
         try {
           if (idf != 0) {
              Log::error("IDF not allowed for Octupus device");
              throw theDationParamSignal;
           }
           if (openParam != 0) {
              Log::error("No open parameters allowed for Octupus device");
              throw theDationParamSignal;
           }
           internalDationOpen();
         } catch (Signal *s) {
            if (rstValue != 0) {
                *rstValue = s->whichRST();
            } else {
              throw;
            }
         }
      }

      /**
      Close the OctopusDigitalOut

      \param closeParam close parameters if given
      \param rstValue pointer to error variable if given


      \throws CloseFailedSignal, if  dation is not opened and rst is not given
      */
      void dationClose(int closeParam=0,
                       Fixed<31>* rstValue=0) {
        try {
           if (closeParam != 0) {
              Log::error("No close parameters allowed for Octupus device");
              throw theDationParamSignal;
           } 
           internalDationClose();
        } catch (Signal *s) {
            if (rstValue != 0) {
                *rstValue = s->whichRST();
            } else {
              throw;
            }
        }
     } 

      /**
      read  a Bit<width> value from the device

      This method will always throw an exception
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws InternalDationSignal, if used at all
      */

      void dationRead(void * data, size_t size);

      /**
      send  a Bit<width> value to the device
      \param data points to the storage location of the data
      \param size denotes the number of bytes of the output data

      \throws DationParamSignal, if size != 1, since 1 byte is
                       expected for the Bit<1..8> value
      \throws DationNotOpen, if  dation is not opened

      */
      void dationWrite(void * data, size_t size);

    /**
    obtain the capabilities of the device

    This method returns :
          OUT ANY

    \returns available commands of the device
    */
    int capabilities();

   };


}
#endif

