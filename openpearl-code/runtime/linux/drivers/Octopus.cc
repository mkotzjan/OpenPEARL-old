

#include "Octopus.h"
#include "Log.h"
#include "Signals.h"

namespace pearlrt {
   Octopus* Octopus::instance = NULL;

   Octopus::Octopus() {
      char desc[64];

      if (! octopus_init(&octopus)) {
         Log::error("Octopus: no board found (%s)", octopus.error_str);
         throw theDationParamSignal;
      }

      if (octopus_open(&octopus) < 0) {
         Log::error("Octopus: failed to open (%s)", octopus.error_str);
         throw theDationParamSignal;
      }

      octopus_get_hwdesc(&octopus, desc);
      Log::info("Octopus board %s ready for use", desc);

      for (int i = 0; i < 6; i++) {
         usedPortBits[i] = 0;
         portValue[i] = 0;
         portDirectionOut[i] = 0;
      }

      Log::info("Octopus initialized");
   }

   Octopus::~Octopus() {
      if (octopus_close(&octopus) < 1) {
         Log::error("Octopus: error closing connection (%s)",
                    octopus.error_str);
         throw theDationParamSignal;
      }

      Log::info("Octopus deinitialized");
   }

   Octopus* Octopus::getInstance() {
      if (! instance) {
         instance = new Octopus();
      }

      return instance;
   }

   void Octopus::usePortBits(char port,
                             int start,
                             int width,
                             OctopusMode mode) {
      int p = port - 'A';
      int mask;

      if (p < 0 || p > 5) {
         Log::error("Octopus: illegal port");
         throw theDationParamSignal;
      }

      if (start > 7 || start < 0) {
         Log::error("Octopus: illegal starting bit number");
         throw theDationParamSignal;
      }

      if (width > 8 || width < 0 || start - width + 1 < 0) {
         Log::error("Octopus: illegal starting bit, width combination");
         throw theDationParamSignal;
      }

      mask = 0x0ff >> (8 - width);
      mask <<= start - width + 1;

      if (usedPortBits[p] & mask) {
         Log::error("Octopus: bits already in use (used %x, requested %x)",
                    usedPortBits[p], mask);
         throw theDationParamSignal;
      }

      usedPortBits[p] |= mask;

      switch (mode) {
      default:
         Log::error("Octopus: error set output dir bits");
         throw theDationParamSignal;

      case DIGOUT:
         portDirectionOut[p] |= mask;

         if (octopus_io_set_port_direction_out(
                  &octopus, p + 1, portDirectionOut[p]) < 0) {
            Log::error("Octopus: error set output dir bits");
            throw theDationParamSignal;
         }

         break;

      case DIGIN:
         // nothing to do
         break;
      }
   }

   void Octopus::freePortBits(char port,
                              int start,
                              int width) {
      int p = port - 'A';
      int mask;
      mask = 0x0ff >> (8 - width);
      mask <<= start - width + 1;
      usedPortBits[p] &= ~mask;
   }

   void Octopus::writePortBits(char port, int start, int width, int value) {
      int p = port - 'A';
      int mask;
      int result;

      // error checking done at usePortBits()
      mask = 0x0ff >> (8 - width);
      mask <<= start - width + 1;
      value <<= start - width + 1;
      value &= mask;
      portValue[p] &= ~mask;
      portValue[p] |= value;

      mutex.lock();
      result = octopus_io_set_port(&octopus, p + 1, portValue[p]);
      mutex.unlock();

      if (result < 0) {
         Log::error("Octopus: error writing bits");
         throw theDationParamSignal;
      }
   }

   int Octopus::readPortBits(char port, int start, int width) {
      int p = port - 'A';
      int mask;
      int value;
      // error checking done at usePortBits()
      mask = 0x0ff >> (8 - width);
      mask <<= start - width + 1;
      mutex.lock();
      value = octopus_io_get_port(&octopus, p + 1);
      mutex.unlock();
      value &= mask;
      value >>= (start - width + 1);
      return value;
   }


}


