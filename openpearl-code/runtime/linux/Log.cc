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
#include <unistd.h>
#include <fcntl.h>

#include "Character.h"
#include "RefChar.h"
#include "Log.h"

namespace pearlrt {
#define ERRORMESSAGE "\n                     **** above line truncated ****\n"

   bool Log::initialized = false;
   int Log::logFileHandle;
   Mutex Log::mutex;

   Log::Log() {
      logFileHandle = open("pearl_log.txt",
                           O_RDWR | O_CREAT | O_TRUNC , 0666
                          );
      initialized = true;
      mutex.name("Log");
   }

   Log* Log::getInstance() {
      static Log* instance = new Log();
      return instance;
   }

   void Log::exit(void) {
      if (initialized) {
         close(logFileHandle);
         initialized = false;
      }
   }

   void Log::doit(const Character<7>& type,
                  const char * format,
                  va_list args) {
      Character<128> line;
      RefCharacter rc(line);

      try {
         doFormat(type, rc, format, args);

         mutex.lock();
         write(logFileHandle, rc.getCstring(), rc.getCurrent());
         mutex.unlock();
      } catch (CharacterTooLongSignal s) {
         mutex.lock();
         write(logFileHandle, line.get(), (size_t)(line.upb().x));
         write(logFileHandle, ERRORMESSAGE, strlen(ERRORMESSAGE));
         mutex.unlock();
      }

      fsync(logFileHandle);
   }


   void Log::info(const char * format, ...) {
      if (logLevel & Log::INFO) {
         Character<7> type("INFO:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::error(const char * format, ...) {
      if (logLevel & Log::ERROR) {
         Character<7> type("ERROR:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::warn(const char * format, ...) {
      if (logLevel & Log::WARN) {
         Character<7> type("WARN:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

   void Log::debug(const char * format, ...) {
      if (logLevel & Log::DEBUG) {
         Character<7> type("DEBUG:");
         va_list args;
         va_start(args, format);
         Log::getInstance()->doit(type, format, args);
         va_end(args);
      }
   }

}
