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

#include "Dation.h"
#include "DationDim.h"
#include "Fixed.h"
#include "Log.h"
#include "Signals.h"
#include "compare.h"

namespace pearlrt {
   const Fixed<31> DationDim::one(Fixed<31>(1));
   const Fixed<31> DationDim::zero(Fixed<31>(0));

   DationDim::DationDim(const Fixed<31> p,
                        const Fixed<31> r,
                        const Fixed<31> c,
                        const int d, const bool b) : pages(p), rows(r),
      cols(c), dimensions(d), boundedDimension(b) {
      row = 0;
      col = 0;
      page = 0;
   }

   void DationDim::setDationParams(const int p) {
      if (!!(p & Dation::CYCLIC) == !!(p & Dation::NOCYCL)) {
         Log::error("DationDim: CYCLIC or NOCYCL required");
         throw theInternalDationSignal;
      }

      if (!!(p & Dation::STREAM) == !!(p & Dation::NOSTREAM)) {
         Log::error("DationDim: STREAM or NOSTREAM required");
         throw theInternalDationSignal;
      }

      dationParams = p;
      reset();
   }

   bool DationDim::checkRemainingWidth(Fixed<15> w) {
      if (dationParams & Dation::STREAM) {
         return true;
      }

      if ((cols < zero).getBoolean()) {
         return true;
      }

      if ((cols - col > w).getBoolean()) {
         return true;
      }

      return false;
   }

   Fixed<31> DationDim::getColumn() const {
      return (col);
   }

   Fixed<31> DationDim::getColumns() const {
      return (cols);
   }

   Fixed<31> DationDim::getRow() const {
      return (row);
   }

   Fixed<31> DationDim::getRows() const {
      return (rows);
   }

   Fixed<31> DationDim::getPage() const {
      return (page);
   }

   Fixed<31> DationDim::getPages() const {
      return (pages);
   }

   int DationDim::getDimensions() const {
      return dimensions;
   }

   void DationDim::reset() {
      if (dationParams & Dation::CYCLIC) {
         if ((cols < zero).getBoolean()) {
            Log::error("DationDim: CYCLIC set for DIM(*) dation");
            throw theInternalDationSignal;
         }

         if (dimensions == 2 && (rows < zero).getBoolean()) {
            Log::error("DationDim: CYCLIC set for DIM(*,N) dation");
            throw theInternalDationSignal;
         }

         if (dimensions == 3 && (pages < zero).getBoolean()) {
            Log::error("DationDim: CYCLIC set for DIM(*,M,N) dation");
            throw theInternalDationSignal;
         }
      }

      page = zero;
      row = zero;
      col = zero;
   }

   Fixed<31> DationDim::getCapacity() const {
      Fixed<31> c(-1);

      switch (dimensions) {
      case 1:
         c = cols;
         break;

      case 2:
         c = rows * cols;
         break;

      case 3:
         c = pages * rows * cols;
         break;
      }

      if ((c < zero).getBoolean()) {
         Log::error("Dation capacity calulation not allowed on "
                    "unlimited dimensions");
         throw theInternalDationSignal;
      }

      return c;
   }

   bool DationDim::isBounded() {
      return boundedDimension;
   }

   void DationDim::gotoNextRecord() {
      col = 0;
      row = row + one;;

      if ((row >= rows).getBoolean()) {
         row = 0;
         page = page + one;
      }

      if (dationParams & Dation::CYCLIC) {
         switch (dimensions) {
         case 1:
            row = 0 ;
            page = 0;
            break;

         case 2:
            if ((page > zero).getBoolean()) {
               page = 0;
               row = 0;
            }

            if ((row >= rows).getBoolean()) {
               row = 0;
            }

            break;

         case 3:
            if ((page >= pages).getBoolean()) {
               row = 0;
               page = 0;
            }

            break;
         }
      }
   }
}
