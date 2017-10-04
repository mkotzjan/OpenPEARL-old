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
#include "DationDim2.h"
#include "Fixed.h"
#include "Log.h"
#include "Signals.h"
#include "compare.h"

namespace pearlrt {

   DationDim2::DationDim2(Fixed<31> c) : DationDim(0, -1, c, 2, false) {
   }

   DationDim2::DationDim2(Fixed<31>r, Fixed<31> c) :
      DationDim(0, r, c, 2, true) {
      if ((r <= zero).getBoolean()) {
         Log::error("DationDim2: rows must be > 0");
         throw theInternalDationSignal;
      }

      if ((c <= zero).getBoolean()) {
         Log::error("DationDim2: cols must be > 0");
         throw theInternalDationSignal;
      }
   }

   Fixed<31> DationDim2::getIndex(const Fixed<31> r,
                                  const Fixed<31> c) const {
      return ((r) * cols +
              (c)
             );
   }

   Fixed<31> DationDim2::getIndex() const {
      return getIndex(row, col);
   }

   void DationDim2::pos(const Fixed<31> r, const Fixed<31> c) {
      if (!(dationParams & Dation::DIRECT)) {
         Log::error("DationDim2: POS needs DIRECT DATION");
         throw theInternalDationSignal;
      }

      if ((c < zero).getBoolean()) {
         Log::error("DationDim2: col POS < 0");
         throw theDationIndexBoundSignal;
      }

      if ((cols > zero).getBoolean() && (c > cols).getBoolean()) {
         Log::error("DationDim2: col POS beyond upper limit");
         throw theDationIndexBoundSignal;
      }

      if ((r < zero).getBoolean()) {
         Log::error("DationDim2: row POS < 0");
         throw theDationIndexBoundSignal;
      }

      if ((rows > zero).getBoolean() && (r > rows).getBoolean()) {
         Log::error("DationDim2: row POS beyond upper limit");
         throw theDationIndexBoundSignal;
      }

      row = r;
      col = c;
   }

   void DationDim2::adv(const Fixed<31> r, const Fixed<31> c) {
      Fixed<31> newCol;
      Fixed<31> newRow;
      Fixed<31> n;
      newRow = row + r;
      newCol = col + c;

      if (((newCol < zero).getBoolean() || (newCol > cols).getBoolean()) &&
            (dationParams & Dation::NOSTREAM)) {
         Log::error("DationDim2: ADV leaves record at NOSTREAM");
         throw theDationIndexBoundSignal;
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
      }

      if (dationParams & Dation::NOCYCL) {
         if ((newRow < zero).getBoolean() ||
               ((rows > zero).getBoolean() &&
                (newRow == rows).getBoolean() &&
                (newCol > zero).getBoolean()) ||
               ((rows > zero).getBoolean() && (newRow > rows).getBoolean())) {
            Log::error("DationDim2: ADV leaves range at NOCYCL");
            throw theDationIndexBoundSignal;
         }
      }

      if (dationParams & Dation::CYCLIC) {
         if ((newRow < zero).getBoolean() ||
               ((rows > zero).getBoolean() &&
                (newRow >= rows).getBoolean())) {
            newRow = newRow % rows;
         }
      }

      if (dationParams & Dation::FORWARD) {
         if ((getIndex(newRow, newCol) < getIndex()).getBoolean()) {
            Log::error("DationDim2: ADV on FORWARD DATION"
                       " needs non negativ offset");
            throw thePositioningFailedSignal;
         }
      }

      row = newRow;
      col = newCol;
   }

   Fixed<31> DationDim2::getElements4Skip(const Fixed<31> n) {
      Fixed<31> result;

      if ((dationParams & Dation::FORWARD) && (n < one).getBoolean()) {
         Log::error("DationDim2: skip(n): n must be >0 on FORWARD dation");
         throw thePositioningFailedSignal;
      }

      result = (cols - col) + (n - one) * cols;
      return result;
   }

}
