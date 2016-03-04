#ifndef RINGBUFFER_INCLUDED
#define RINGBUFFER_INCLUDED

namespace pearlrt {

/**
A generic ring buffer with non blocking behavior

*/
template <class T> class RingBuffer {

   private:
      T* data;
      int filled;
      int size;
      int writeIndex;
      int readIndex;
   public:
      RingBuffer() {
          data = NULL;
          filled=0;
          size = 0;
          writeIndex=0;
          readIndex=0;
      }

      void setup(T* d, int s) {
         data = d;
         size = s;
      }
      

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

       bool last(T* d) {
         if (filled  > 0) {
            *d = data[(writeIndex-1)%size];
            return true;
         }
         // else signal that no data is available
         return false;
       }

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

