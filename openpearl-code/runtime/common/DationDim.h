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

#ifndef DATIONDIM_INCLUDED
#define DATIONDIM_INCLUDED

#include "Fixed.h"
#include "Dation.h"

namespace pearlrt {
   /**
   \addtogroup io_common_helper
   @{
   */

   /**
   \brief parent class for all possible dations dimensions

   PEARL allows dations to be organised in tables of
   <ul>
   <li>pages
   <li>rows
   <li>columns
   </ul>

   Not all dimensions must be used.
   As compiler API different dimensioning classes are provided.
   They are derived from this class.
   Each user dation needs a dimension specification.

   Each specilized dation dimension object is responsible to check
   the validity of positioning according the given dimensions and
   of the dation attributes STREAM/NOSTREAM and CYCLIC/NONCYCLIC.

   The internal values for column, row and page number start at 0.
   The correction to the PEARL specific values 1.. must be done in another
   module (e.g. UserDation).

   This class contains the dimension range for all 3 dimensions
   for simplicity in structure.

   The values col, row and page denote the location to be read/written
   with the next statement. It is ok that this location reaches end
   of record. The following read/write will call the adv() method to
   update the simulated location before trying to read or write. This
   will produce the signals if dation boundaries are violated.

   */
   class DationDim {
   public:
      /**
      calculate the current index position within the data structure

      \returns the current index (starting at 0 for the first element)
      */
      virtual Fixed<31> getIndex() const = 0;

      /**
      calculate the number of elements of the dation dimension

      This is only possible, if the dimension ist limited (not *).

      \throws InternalDationSignal, if the dation dimension is not limited

      \returns the total number of elements in the dation dimension
      */
      Fixed<31> getCapacity() const;

      /**
      return the number of defined dimensions
      \returns the number of dimensions (0, if none)
      */
      int getDimensions() const;


   protected:
      /** constructor, just to allow the preset of pages, rows, cols
         and dimensions

         \param p the number of pages, if >0; or -1 if not specified
         \param r the number of rows, if >0; or -1 if not specified
         \param c the number of colums, if >0; or -1 if not specified
         \param d the number of dimensions; must be 1,2 or 3
         \param b denote if the dimension is bounded
      */
      DationDim(const Fixed<31> p, const Fixed<31>r, const Fixed<31> c,
                const int d, const bool b);

      /** number of pages if >0; -1 denote DIM(*,N,M) */
      const Fixed<31> pages;

      /** number of rows if >0; -1 denote DIM(*,N) */
      const Fixed<31> rows;

      /** number of columns if >0; -1 denote DIM(*) */
      const Fixed<31> cols;

   private:
      /** number of dimensions */
      const int dimensions;

      /** is bound  */
      const bool boundedDimension;

   protected:
      /** current page number */

      Fixed<31> page;

      /** current row number */
      Fixed<31> row;

      /** current column number */
      Fixed<31> col;

      /**
      offset constant needed to transform PEARL coordinates 1.. to native
      coordinates 0..
      */
      static const Fixed<31> one;

      /**
      Fixed<31> constant with value 0
      */
      static const Fixed<31> zero;

      /**
      dation parameters (NO)STREAM/(NO)CYCLIC to realize dimension
      border behavior
      */
      int dationParams;

   public:
      /**
      set dation parameters for (NO)STREAM/(NO)CYCLIC

      \param p the attributes of the dation (STREAM, CYCLIC)
      */
      void setDationParams(const int p);

      /**
      get current column position

      \returns current column position
      */
      Fixed<31> getColumn() const;

      /**
      get number of columns

      \returns number of columns in dation
      */
      Fixed<31> getColumns() const;

      /**
      get current row position

      \returns current row position
      */
      Fixed<31> getRow() const;

      /**
      get number of rows

      \returns number of rows in dation
      */
      Fixed<31> getRows() const;

      /**
      get current page position

      \returns current page position
      */
      Fixed<31> getPage() const;

      /**
      get number of pages

      \returns number of pages in dation
      */
      Fixed<31> getPages() const;

      /**
      test remaining width

      \param w minimum required within the current row

      \returns true, if a field of given width fits into the current row, or
                    STREAM is set

      \returns false, if the remaining width is less than w and NOSTREAM is set
      */
      bool checkRemainingWidth(Fixed<15> w);

      /**
      reset position
      */
      void reset();

      /**
      tests whether the dimsion object is bound

      \returns true, if the all dimension attributes are set<br>
               false, if the upmost dimension is not specified
      */
      bool isBounded();

      /**
      set position without bound checking to the start of the next record

      This method is necessary when read or write ends at the end of the file
      dimension with TFU or trailing skip.
      As long as no further reading/writing occurs this is ok.
      */
      void gotoNextRecord();
   };
   /** @} */
}
#endif

