/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
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

/*
 * Date.h
 *
 *  Created on: 03.05.2014
 *      Author: florian
 * renamed and extended to RTC mapper
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include <sys/time.h>



namespace pearlrt {
   /**
   provide access to the real time clock of the lpc1768 controller
   */
   class RTC {


   public:
      /********************************************************************
       * @brief            convert UTC time to unix time format
       * @param[in]        year
       * @param[in]        month
       * @param[in]        day
       * @param[in]        hour
       * @param[in]        minute
       * @param[in]        second
       * @return           none
       **********************************************************************/
      static int64_t getUnixTimeStamp(int year, int month, int day,
                                      int hour, int minute, int second);

      /********************************************************************
       * @brief            get the current system time in unix format from RTC
       * @param[in]        tv timeval structure
       *                   (see http://linux.die.net/man/2/gettimeofday)
       * @return           none
       **********************************************************************/
      static void getTimeOfDay(timeval* tv);

      /*******************************************************************
       * @brief            setup the RTC if not yet none
       * @param[in]        none
       * @return           none
       **********************************************************************/
      static void init();
   };

// namespace end
}



#endif /* RTC_H_ */
