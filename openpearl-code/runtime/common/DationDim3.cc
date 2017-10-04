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
#include "DationDim3.h"
#include "Fixed.h"
#include "Log.h"
#include "Signals.h"
#include "compare.h"

namespace pearlrt {

   DationDim3::DationDim3(Fixed<31> r, Fixed<31> c) :
      DationDim(-1, r, c, 3, false) {
   }

   DationDim3::DationDim3(Fixed<31> p, Fixed<31>r, Fixed<31> c) :
      DationDim(p, r, c, 3, true) {
      if ((p <= zero).getBoolean()) {
         Log::error("DationDim3: pages must be > 0");
         throw theInternalDationSignal;
      }

      if ((r <= zero).getBoolean()) {
         Log::error("DationDim3: rows must be > 0");
         throw theInternalDationSignal;
      }

      if ((c <= zero).getBoolean()) {
         Log::error("DationDim3: cols must be > 0");
         throw theInternalDationSignal;
      }
   }

   Fixed<31> DationDim3::getIndex(const Fixed<31> p,
                                  const Fixed<31> r,
                                  const Fixed<31> c) const {
      return ((p * rows + r) * cols + c);
   }

   Fixed<31> DationDim3::getIndex() const {
      return getIndex(page, row, col);
   }

   void DationDim3::pos(const Fixed<31> p,
                        const Fixed<31> r,
                        const Fixed<31> c) {
      if (!(dationParams & Dation::DIRECT)) {
         Log::error("DationDim3: POS needs DIRECT DATION");
         throw theInternalDationSignal;
      }

      if ((c < zero).getBoolean()) {
         Log::error("DationDim3: col POS < 0");
         throw theDationIndexBoundSignal;
      }

      if ((cols > zero).getBoolean() && (c > cols).getBoolean()) {
         Log::error("DationDim3: col POS beyond upper limit");
         throw theDationIndexBoundSignal;
      }

      if ((r < zero).getBoolean()) {
         Log::error("DationDim3: row POS < 0");
         throw theDationIndexBoundSignal;
      }

      if ((rows > zero).getBoolean()  && (r > rows).getBoolean()) {
         Log::error("DationDim3: row POS beyond upper limit");
         throw theDationIndexBoundSignal;
      }

      if ((p < zero).getBoolean()) {
         Log::error("DationDim3: page POS < 0");
         throw theDationIndexBoundSignal;
      }

      if ((pages > zero).getBoolean() && (p > pages).getBoolean()) {
         Log::error("DationDim3: page POS beyond upper limit");
         throw theDationIndexBoundSignal;
      }

      page = p;
      row = r;
      col = c;
   }

   void DationDim3::adv(const Fixed<31> p,
                        const Fixed<31> r,
                        const Fixed<31> c) {
      Fixed<31> newCol;
      Fixed<31> newRow;
      Fixed<31> newPage;
      Fixed<31> n;
      newPage = page + p;
      newRow = row + r;
      newCol = col + c;

      if (dationParams & Dation::NOSTREAM) {
         if (((newCol < zero).getBoolean() || (newCol > cols).getBoolean())) {
            Log::error("DationDim3: ADV leaves record at NOSTREAM");
            throw theDationIndexBoundSignal;
         }

         if ((newRow < zero).getBoolean() ||
               (newRow > rows).getBoolean() ||
               ((newRow == rows).getBoolean() &&
                (newCol > zero).getBoolean())) {
            Log::error("DationDim3: ADV leaves record at NOSTREAM");
            throw theDationIndexBoundSignal;
         }
      }

      if (dationParams & Dation::STREAM) {
         if ((newCol < zero).getBoolean()) {
            n = (-newCol) / cols + one;
            newCol = newCol + n * cols;
            newRow = newRow - n;
         } else if ((newCol >= cols).getBoolean()) {
            n = newCol / cols;
            newCol = newCol - n * cols;
            newRow = newRow + n;
         }

         if ((newRow < zero).getBoolean()) {
            n = (-newRow) / rows + one;
            newRow = newRow + n * rows;
            newPage = newPage - n;
         } else if ((newRow >= rows).getBoolean()) {
            n = newRow / rows;
            newRow = newRow - n * rows;
            newPage = newPage + n;
         }
      }

      if (dationParams & Dation::NOCYCL) {
         if ((newPage < zero).getBoolean() ||
               ((pages > zero).getBoolean() &&
                (newPage == pages).getBoolean() &&
                ((newRow > zero).getBoolean() || (newCol > zero).getBoolean())
               ) ||
               ((pages > zero).getBoolean() &&
                (newPage > pages).getBoolean())) {
            Log::error("DationDim3: ADV leaves range at NOCYCL");
            throw theDationIndexBoundSignal;
         }
      }

      if (dationParams & Dation::CYCLIC) {
         if ((newPage < zero).getBoolean() ||
               ((pages > zero).getBoolean() &&
                (newPage >= pages).getBoolean())) {
            newPage = newPage % pages;
         }
      }

      if (dationParams & Dation::FORWARD) {
         if ((getIndex(newPage, newRow, newCol) < getIndex()).getBoolean()) {
            Log::error("DationDim3: ADV on FORWARD DATION"
                       " needs non negativ offset");
            throw theInvalidPositioningSignal;
         }
      }

      page = newPage;
      row = newRow;
      col = newCol;
   }

   Fixed<31> DationDim3::getElements4Skip(const Fixed<31> n) {
      Fixed<31> result;

      if (dationParams & Dation::FORWARD && (n < one).getBoolean()) {
         Log::error("DationDim3: skip(n): n must be >0 on FORWARD dation");
         throw theInvalidPositioningSignal;
      }

      result = (cols - col) + (n - one) * cols;
      return result;
   }

   Fixed<31> DationDim3::getElements4Page(const Fixed<31> n) {
      Fixed<31> result;

      if (dationParams & Dation::FORWARD && (n < one).getBoolean()) {
         Log::error("DationDim3: page(n): n must be >0 on FORWARD dation");
         throw theInvalidPositioningSignal;
      }

      result = (cols - col) + (rows - row - one) * cols;
      result = result + (n - one) * rows * cols;
      return result;
   }
}
