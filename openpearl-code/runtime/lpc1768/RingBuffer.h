/*
 [A "BSD license"]
 Copyright (c) 2016 Rainer Mueller
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

#ifndef RINGBUFFER_INCLUDED
#define RINGBUFFER_INCLUDED

namespace pearlrt {

/**
A generic ring buffer with non blocking behavior

\tparam T the type of data which is inside the ring buffer

*/
template <class T> class RingBuffer {

   private:
      T* data;
      int filled;
      int size;
      int writeIndex;
      int readIndex;
   public:
      /**
        initialise the ring buffer administration data
      */
      RingBuffer() {
          data = NULL;
          filled=0;
          size = 0;
          writeIndex=0;
          readIndex=0;
      }

      /**
        associate the ring buffer with the data storage 

        \param d pointer to the data storage of the ring buffer
        \param s number of elements in the ring buffer
      */
      void setup(T* d, int s) {
         data = d;
         size = s;
      }
      
      /**
      add one element to the ring buffer

      \param d the element which should be added
      \returns  true, if element was added
      \returns  false, if buffer is full - the eldest element was discarded
      */
      bool add(T d) {
         if (filled  < size) {
            data[writeIndex++] = d;
            writeIndex %= size;
            filled ++;
            return true;
         } else {
            // buffer full; forget eldest entry
            data[writeIndex++] = d;
            writeIndex %= size;
            readIndex++;
            readIndex %= size;
            return false;
         }
      }

       /**
       read the eldest element from the ring buffer

       \param d pointer, where the data should be copied to
       \returns true, if data were available
       \returns flase, if no data were available - no data copied
       */
       bool get(T* d) {
         if (filled  > 0) {
            *d = data[readIndex++];
            readIndex %= size;
            filled --;
            return true;
         }
         // else signal that no data is available
         return false;
       }

       /**
       read the newest element from the ring buffer

       \param d pointer, where the data should be copied to
       \returns true, if data were available
       \returns flase, if no data were available - no data copied
       */
       bool last(T* d) {
         if (filled  > 0) {
            *d = data[(writeIndex-1)%size];
            return true;
         }
         // else signal that no data is available
         return false;
       }

       /**
          forget the newest element in the ring buffer

          if no data are in the buffer - nothing happens
       */
       void forget() {
         if (filled  > 0) {
            writeIndex--;
            writeIndex %= size;
            filled --;
         } 

       }
};

};
#endif

