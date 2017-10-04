/*
 [The "BSD license"]
 Copyright (c) 2012-2014 Rainer Mueller
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

#ifndef OCTOPUS_H_INCLUDED
#define OCTOPUS_H_INCLUDED

#include <octopus.h>
/**
\file

\brief Coordination of I/O-usage on the Octipus-I/O-board
*/
#include "Mutex.h"

namespace pearlrt {
   /**
   This class provides the basic access to the octupus i/o-board
   of the company Embedded Projects.

   There is booking mechanisms to monitor the
   correct usage of the octopus i/o-board as well as i/o-primitves.

   In case of multiple usage of i/o-connections a PEARL signal
   is raised.

   Currently only one octupus board is supported.
   */
   class Octopus {
   private:
      Octopus();
      ~Octopus();
      static Octopus* instance;
      struct octopus_context octopus;
      char usedPortBits[6];
      char portValue[6];
      char portDirectionOut[6];
      Mutex mutex;
   public:
      /**
         supported modes of the i/o-usage

         <ul>
         <li>DIGIN provides digital input mode
         <li>DIGOUT provides digital output mode
         </ul>
      */
      enum OctopusMode {DIGOUT, DIGIN};

      /**
      get access to the monitoring object (singleton pattern)

      \returns pointer to the (one and only) minitoring object
      */
      static Octopus* getInstance();

      /**
      mark the given bits as used

      \param port is the port character 'A', 'B', ...'E'
      \param start is the starting bit number (7..0)
      \param width is the number of bits (1..8)
      \param mode marks the kind of usage (DIGIN, DIGOUT, ...)

      \throws DationParamSignal if a conflict is detected
      */
      void usePortBits(char port, int start, int width, OctopusMode mode);

      /**
      mark the given bits as free

      This method is only needed for testing purpose.
      PEARL has static system dations. The free is called
      at the end of the application, when the dtor of the
      system dation object is called.

      \param port is the port character 'A', 'B', ...'E'
      \param start is the starting bit number (7..0)
      \param width is the number of bits (1..8)

      */
      void freePortBits(char port, int start, int width);

      /**
      write a digital value to the given part of the port

      This method updates an internal register to avoid
      collissions between separate dations on the same
      port of the i/o-board.

      \param port is the port character 'A', 'B', ...'E'
      \param start is the starting bit number (7..0)
      \param width is the number of bits (1..8)
      \param value contains the value, which shall be written
                   on the least significant bits

      \note Does no throw exceptions in case of misuse. The parameters
            port, startBit and width must be according the
            usePortBits()-call
      */
      void writePortBits(char port, int start, int width, int value);

      /**
      read a digital value from the given part of the port

      This method selects the requested part of the input-port
      and returns them as the least significant bits of the
      return value.

      \param port is the port character 'A', 'B', ...'E'
      \param start is the starting bit number (7..0)
      \param width is the number of bits (1..8)
      \returns the value, which shall be read
                   on the least significant bits

      \note Does no throw exceptions in case of misuse. The parameters
            port, startBit and width must be according the
            usePortBits()-call
      */
      int readPortBits(char port, int start, int width);
   };
}
#endif

