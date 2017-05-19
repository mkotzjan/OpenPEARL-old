#include "Signals.h"
#include "Log.h"

namespace pearlrt {
   template<class C> C& Cont(C* x) {
      if (x) {
         return *x;
      }
      Log::error("use of uninitialized reference");
      throw theRefNotInitializedSignal; 
   }
}

