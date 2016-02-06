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

#ifndef LOG_INCLUDED
#define LOG_INCLUDED

/**
\file

\brief simple logging system

This module realizes a simple log facility the the PEARL runtime system.
Depending on a statically set log-level, all incoming messages will
be written to a log file, if the current log-level matches the given
log request

Each log-request corresponds with a bit in log-level. If the bit
is set, the message will be formatted and sent to the log file.

The implementation is thread and signal safe. This is achieved by the
usage of open/close/write function. The formatting is provided as
own code, sonce printf() is known to be non signal safe.
If the maximum text width is exceeded the output is truncated
at the maximum position.

The following formats are supported - all without width specification:
<ul>
<li>%d  - signed int
<li>%x  - int
<li>%u  - unsigned int
<li>%s  - char * (c-strings)
<li>%f  - doubles (including specification of the prcision like %.3f)
<li>%c  - single char
</ul>
*/

#include <cstdarg>    // va_list
#include "Character.h"
#include "Mutex.h"  // write is not thread safe -- mutex required

namespace pearlrt {

   /**
      This class provides some methods to write messages to a log file.
      Each message will be written with a time stamp to a file in the
      current directory.

      \todo realize configurable file name using
            a configuration file
   */
   class Log {
   public:
      /**
        constants for log level setting
      */
      enum LogLevel {DEBUG = 1, INFO = 2, WARN = 4, ERROR = 8};
   private:
      Log();
      /**
         singleton pattern resolve static initialize problem
      */
      static Log* getInstance();
      static int logFileHandle;
      static int logLevel;
      static bool initialized;
      static Mutex mutex;

   public:
      /**
      write a log message

      \param message the message to be written
      */
      static void doit(const Character<7>& type, const char * format,
                       va_list args);
    private:
      /**
       the task which formats the message when scheduler is running
 
       \param p dummy parameter, just to satisfy FreeRTOS
      */
      static void logTask(void * p);

      /**
      write a log message

      \param message the message to be written
      */
      static void doitSync(const Character<7>& type, const char * format,
                       va_list args);
   public:
      /**
      write an info log message with parameters

      \param format the message format (like in printf) to be written
      \param ... as required in format
      */
      static void info(const char * format, ...)
      __attribute__((format(printf, 1, 2)));
      /**
      write an error log message

      \param format the message format (like in printf) to be written
      \param ... as required in format
      */
      static void error(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      /**
      write a warning log message

      \param format the message format (like in printf) to be written
      \param ... as required in format
      */
      static void warn(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      /**
      write a debugging log message

      \param format the message format (like in printf) to be written
      \param ... as required in format
      */
      static void debug(const char * format, ...)
      __attribute__((format(printf, 1, 2)));

      /**
      close the logging system
      */
      static void exit();

      /**
       set log level

      \param level a binary coded value from the enumeration LogLevels
      */
      static void setLevel(int level);
   };

}

#endif

