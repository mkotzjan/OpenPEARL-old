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

#ifndef SIGNALS_INCLUDES
#define SIGNALS_INCLUDES

#include <stdlib.h>

/**
\file
\brief The base class for all PEARL signals.

*/

namespace pearlrt {

   /**
   PEARL signals are a kind of exceptions.
   To capture several SIGNALs of the same kind,
   the exceptions are organized as a class hierarchy.
   The base class provides a method to identify the currently raised SIGNAL.
   The base class also provide a method to get an RST(failure) number,
   necessary for dations.

   \bug forward declaration for fixed and signals failed, probably of
        templates in fixed (Signals need Fixed and Fixed need Signals).
        Therefore actually RST is type int and not Fixed<15>-
   */
   class Signal {
   private:
      static Signal ** signalVector;
      static size_t nbrOfSignals;

   public:
      /**
      Function to throw a Signal identified
      by rst num

      \param rst number of the signal to be thrown
      */
      static void throwSignalByRst(int rst);

   protected:
      char* type;	 //!< the current signal text
      int rstNum;	 //!< the current signal number
      int currentRst;	 //!< the signal number during INDUCE .. RST(..)
   public:
      /**
      Base class of all SIGNALS
      */
      Signal();

      /**
      identification method of the raised signal

      \returns the display text for the raised signal
      */
      const char* which(void);

      /**
      identification method of the raised signal

      \returns RST num(failure number) of the signal
      */
      int whichRST(void);

      /**
      induce with default RST-value
      */
      void induce(void);

      /**
      induce with special RST-value

      This special RST value superseeds the default RST value
      until this signal is caught.

      \param r the rst-value of the signal to be induced
      */
      void induce(int r);

      /**
      get current RST-value

      \returns the current RST vakue of the signal
      */
      int getCurrentRst(void);
   };

   /* the next include is not passed to doxygen */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "Signals.hh"
#endif
}

#endif
