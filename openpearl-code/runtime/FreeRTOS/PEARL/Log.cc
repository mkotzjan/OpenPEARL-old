/*
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

#include <cstdarg>   // for va_start,..

#include "Clock.h"
#include "PutClock.h"
#include "Character.h"
#include "RefChar.h"
#include "RefCharSink.h"
#include "Log.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"	// FreeRTOS functions

extern "C" {
   extern int _write(int, const void* data, int size);
};

#define LOGTASK
#define ERRORMESSAGE "\n                     **** above line truncated ****\n" 

namespace pearlrt {

   bool Log::initialized = false;
   Mutex Log::mutex;
#ifdef LOGTASK
// formatting an output is done during in separate task if scheduler
// is running. This safes a lot of tasks stack
#define LOGSTACKSIZE 400
   static TCB_t        logTcb;
   static StackType_t  logStack[LOGSTACKSIZE];
   static SemaphoreHandle_t logDone, logBusy, logGo;
   static TaskHandle_t      logTaskHandle;
   static const Character<7>* commonType;
   static const char * commonFormat;
   static va_list commonArgs;
   static bool schedulerWasStarted = false;
#endif

#define ERRORMESSAGE "\n                     **** above line truncated ****\n"

   void Log::logTask(void * p) {
#ifdef LOGTASK
      schedulerWasStarted = true;

      while (1) {

         // wait for new log job
         xSemaphoreTake(logGo, portMAX_DELAY);

         // new job arrived
         Log::doitSync(*commonType, commonFormat, commonArgs);

         // and tell application task that the log job was done
         xSemaphoreGive(logDone);
      }

#endif
   }

   Log::Log() {
      StructParameters_t createParameters; // for task creation
      initialized = true;
      mutex.name("Log");

#ifdef LOGTASK
      createParameters.pvParameter = NULL;
      createParameters.stack       = logStack;
      createParameters.tcb         = &logTcb;

      // set the priority to the same as the idle tasks priority
      // the tasks priority will follow the calling tasks priority
      xTaskCreate(logTask, "Log", LOGSTACKSIZE, &createParameters,
                  0, &logTaskHandle);

      logBusy = xSemaphoreCreateMutex();
      logDone  = xSemaphoreCreateBinary();
      logGo    = xSemaphoreCreateBinary();
#endif
   }

   Log* Log::getInstance() {
       static Log * instance = new Log();
       return (Log*) instance; 
   }

   void Log::doit(const Character<7>& type,
                  const char * format,
                  va_list args) {
#ifdef LOGTASK

      if (schedulerWasStarted) {

         // only 1 log job possible at one time
         xSemaphoreTake(logBusy, portMAX_DELAY);
         {
            commonType = &type;
            commonFormat = format;
            commonArgs = args;

            // set the priority of the log task to the same as the calling task
            vTaskPrioritySet(logTaskHandle, uxTaskPriorityGet(NULL));

            // start to log job
            xSemaphoreGive(logGo);

            // wait until log job is finished
            xSemaphoreTake(logDone, portMAX_DELAY);
         }
         xSemaphoreGive(logBusy);

      } else {
#endif
         doitSync(type, format, args);
#ifdef LOGTASK
      }

#endif
   }

   void Log::doitSync(const Character<7>& type,
                      const char * format,
                      va_list args) {
      Character<128> line;
      RefCharacter rc(line);
     
      try {
         doFormat(type, rc, format, args);

         mutex.lock();
         _write(1, rc.getCstring(), rc.getCurrent());
         mutex.unlock();

      } catch (CharacterTooLongSignal s) {
         mutex.lock();
         _write(1, line.get(), sizeof(line));
         _write(1, ERRORMESSAGE, strlen(ERRORMESSAGE));
         mutex.unlock();
      } catch (...) {
         _write(1, "Log got other signal\n", 21);
      }
   }

   void Log::exit(void) {
      if (initialized) {
         initialized = false;
      }
   }


   void Log::info(const char * format, ...) {
      if (logLevel & Log::INFO) {
         static const Character<7> type("INFO:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::error(const char * format, ...) {
      if (logLevel & Log::ERROR) {
         static const Character<7> type("ERROR:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::warn(const char * format, ...) {
      if (logLevel & Log::WARN) {
         static const Character<7> type("WARN:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::debug(const char * format, ...) {
      if (logLevel & Log::DEBUG) {
         static const Character<7> type("DEBUG:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

}
