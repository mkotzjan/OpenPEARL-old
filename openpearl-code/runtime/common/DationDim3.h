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

#ifndef DATIONDIM3_INCLUDED
#define DATIONDIM3_INCLUDED

#include "Fixed.h"
#include "Dation.h"
#include "DationDim.h"

namespace pearlrt {
   /**
   \addtogroup io_common
   @{
   */

   /**
   \brief 3 dimensional dation.

   This class provides constructors and positioning methods. For details
   see: DationDim.h

   */
   class DationDim3 : public DationDim {
   private:
      /**
      calculate the current index position from the gien parameters

      \param p page index
      \param r row index
      \param c column index

      \returns the current index (starting at 0 for the first element)
      */
      Fixed<31> getIndex(const Fixed<31> p,
                         const Fixed<31> r,
                         const Fixed<31> c) const;


   public:
      /**
      calculate the current index position within the data structure

      \returns the current index (starting at 0 for the first element)
      */
      Fixed<31> getIndex() const;

      /**
      setup 3 dimensional unbounded dim
      \param r number of rows
      \param c number of cols
      */

      DationDim3(Fixed<31> r, Fixed<31> c);
      /**
      setup 3 dimensional bounded dim
      \param p number of pages
      \param r number of rows
      \param c number of cols
      */
      DationDim3(Fixed<31> p, Fixed<31> r, Fixed<31> c);

      /**
      absolute POS format

      \param c the new absolute col position
      \param r the new absolute row position
      \param p the new absolute page position
      \throws InternalDationSignal if no DIRECT dation
      \throws DationIndexBoundsSignal if new position would be out of bounds
      */
      void pos(const Fixed<31> p, const Fixed<31> r, const Fixed<31> c);

      /**
      ADV-format

      relative positioning format; needs DIM


      \param p the number of pages to be moved (<0 if backward)
      \param r the number of rows to be moved (<0 if backward)
      \param c the number of cols to be moved (<0 if backward)
      \throws InternalDationSignal if no DIRECT dation
      \throws DationIndexBoundsSignal if new position would be out of bounds
      */
      void adv(const Fixed<31> p, const Fixed<31> r, const Fixed<31> c);

      /**
      return the number of elements for a skip statement

      This method calculates the number of remaining elements
      in the current row and adds the elements on (n-1) complete rows

      \param n number of rows to skip, must be >0

      \throw DationParamSignal, if n <= 0

      \returns number of elements to beginning of n-th next row
      */
      Fixed<31> getElements4Skip(const Fixed<31> n);

      /**
      return the number of elements for a page statement

      This method calculates the number of remaining elements
      in the current row and adds the elements for the
      remaining rows on the page and add (n-1) complete pages

      \param n number of pages to skip, must be >0

      \throw DationuParamSignal, if n <= 0

      \returns number of elements to beginning of n-th next row
      */
      Fixed<31> getElements4Page(const Fixed<31> n);

   };
   /** @} */
}
#endif

