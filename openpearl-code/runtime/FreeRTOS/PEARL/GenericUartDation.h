#ifndef GENERICUARTDATION_INCLUDED
#define GENERICUARTDATION_INCLUDED

namespace pearlrt {

   class GenericUartDation {
      public:
         /**
           pass the received character to the actice dation input buffer.
         
          \param ch the received character
          \returns true, if the character is stored in the input buffer
          \returns false, if there is no input request pending.
         */
         virtual bool addReceivedChar(char ch)=0;

         /**
         retrieve the next character from an output request 
         
         \param ch pointer to the storage for the next character to be
                   transmitted
         \returns true, if there was a character to transmit
         \returns false, if no output data is available
         */
         virtual bool getNextTransmitChar(char * ch)=0;
   };


}
#endif
