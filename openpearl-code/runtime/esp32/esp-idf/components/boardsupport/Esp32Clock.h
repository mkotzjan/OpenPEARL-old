/*
 [A "BSD license"]
 Copyright (c) 2015-2017 Rainer Mueller
 Copyright (c) 2018 Michael Kotzjan
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

#ifndef ESP32_CLOCK_INCLUDED
#define ESP32_CLOCK_INCLUDED


/**
\file

\brief select the clock source by defining a system device
*/


namespace pearlrt {

   /**
   \brief Clock Source for the ESP32

   With this class it is possible to define the clock source
   for an application. The parameter selects one of the possible
   clock sources.

   The default clock Esp32Clock(0) is set automatically in
   system startup procedure, if no other setting was selected.

   Usage:
   \verbatim
   SYSTEM;
      Esp32Clock(1); ! 0=Systick, 1=RTC initial time + Systick
   PROBLEM;
      ! no access to this device in the problem part
   \endverbatim

   */
   class Esp32Clock {

   private:
      static bool clockSelected;

   public:

      /**
      Constructor to setup the clock

      \param typeOfClock selector of the desired clock system
                  <ul>
        <li>0=Systick only (no absolute time),
                  <li>1=RTC used as initial time; update by systick
                  <li> ... others follow
                  </ul>

      */
      Esp32Clock(const int typeOfClock);

      /**
      check if a clock was selected

      \return true, if a clock source was selected
      */
      static bool isClockSelected();
   };
}
#endif
