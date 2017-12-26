/*
 [A "BSD license"]
 Copyright (c) 2014-2017 Rainer Mueller
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

/**
\file
\brief implementation of  I/O-Formats for PG-userdation and CONVERT

*/
#include "IOFormats.h"
#include "Sink.h"
#include "Source.h"
#include "Fixed.h"
#include "PutHelper.h"
#include "PutClock.h"
#include "GetClock.h"
#include "PutDuration.h"
#include "GetDuration.h"

namespace pearlrt {

   void IOFormats::setupIOFormats(Sink * _sink, Source * _source) {
      sink = _sink;
      source = _source;
   }

   void IOFormats::toA(void *s, size_t len, Fixed<31> w) {
      checkCapacity(w);
      RefCharacter rc;
      rc.setWork(s, len);
      PutHelper::doPutChar(w.x, &rc, sink);
   }

   void IOFormats::fromA(void *s, size_t len, Fixed<31> w) {
      checkCapacity(w);
      GetHelper helper(w, source);
      helper.setDelimiters(GetHelper::EndOfLine);

      RefCharacter rc;
      rc.setWork(s, len);

      helper.readCharacterByA(&rc);
   }

   void IOFormats::toFloatF(void *s, size_t index,
                            size_t len,
                            const Fixed<31> w,
                            const Fixed<31> d) {
      checkCapacity(w);

      if (len == 24) {
         Float<24> * f = (Float<24>*)s;
         f += index;
         PutFloat<24>::toF(*f, w, d, *sink);
      } else if (len == 53) {
         Float<53> * f = (Float<53>*)s;
         f += index;
         PutFloat<53>::toF(*f, w, d, *sink);
      } else {
         Log::error("unsupported length of float F-format (len=%zu)", len);
         throw theInternalDationSignal;
      }
   }

   void IOFormats::fromFloatF(void *s, size_t index,
                            size_t len,
                            const Fixed<31> w,
                            const Fixed<31> d) {
      checkCapacity(w);

      if (len == 24) {
         Float<24> * f = (Float<24>*)s;
         f += index;
         GetFloat<24>::fromF(*f, w, d, *source);
      } else if (len == 53) {
         Float<53> * f = (Float<53>*)s;
         f += index;
         GetFloat<53>::fromF(*f, w, d, *source);
      } else {
         Log::error("unsupported length of float F-format (len=%zu)", len);
         throw theInternalDationSignal;
      }
   }

   void IOFormats::toFixedF(void *s, size_t index,
                            size_t len,
                            const Fixed<31> w,
                            const Fixed<31> d) {
      checkCapacity(w);

      if (len <= 7) {
         Fixed<7> * f = (Fixed<7>*)s;
         f += index;
         PutFixed<7>::toF(*f, w, d, *sink);
      } else if (len <= 15) {
         Fixed<15> * f = (Fixed<15>*)s;
         f += index;
         PutFixed<15>::toF(*f, w, d, *sink);
      } else if (len <= 31) {
         Fixed<31> * f = (Fixed<31>*)s;
         f += index;
         PutFixed<31>::toF(*f, w, d, *sink);
      } else if (len <= 63) {
         Fixed<63> * f = (Fixed<63>*)s;
         f += index;
         PutFixed<63>::toF(*f, w, d, *sink);
      } else {
         Log::error("unsupported length of fixed F-format (len=%zu)", len);
         throw theInternalDationSignal;
      }
   }

   void IOFormats::fromFixedF(void *s, size_t index,
                            size_t len,
                            const Fixed<31> w,
                            const Fixed<31> d) {
      checkCapacity(w);

      if (len <= 7) {
         Fixed<7> * f = (Fixed<7>*)s;
         f += index;
         GetFixed<7>::fromF(*f, w, d, *source);
      } else if (len <= 15) {
         Fixed<15> * f = (Fixed<15>*)s;
         f += index;
         GetFixed<15>::fromF(*f, w, d, *source);
      } else if (len <= 31) {
         Fixed<31> * f = (Fixed<31>*)s;
         f += index;
         GetFixed<31>::fromF(*f, w, d, *source);
      } else if (len <= 63) {
         Fixed<63> * f = (Fixed<63>*)s;
         f += index;
         GetFixed<63>::fromF(*f, w, d, *source);
      } else {
         Log::error("unsupported length of fixed F-format (len=%zu)", len);
         throw theInternalDationSignal;
      }
   }

   void IOFormats::toBit(void *s, size_t index,
                            size_t len, int base,
                            const Fixed<31> w) {
      checkCapacity(w);

      if (len <= 8) {
         BitString<8> * f = (BitString<8>*)s;
         f += index;
         PutBits<1>::toBit(f->x, len, w.x, base, *sink);
      } else if (len <= 15) {
         BitString<16> * f = (BitString<16>*)s;
         f += index;
         PutBits<2>::toBit(f->x, len, w.x, base, *sink);
      } else if (len <= 31) {
         BitString<32> * f = (BitString<32>*)s;
         f += index;
         PutBits<4>::toBit(f->x, len, w.x, base, *sink);
      } else if (len <= 63) {
         BitString<64> * f = (BitString<64>*)s;
         f += index;
         PutBits<8>::toBit(f->x, len, w.x, base, *sink);
      } else {
         Log::error("unsupported length of fixed B-format (len=%zu)", len);
         throw theInternalDationSignal;
      }
   }

#if 0
   void IOFormats::fromBit(void *s, size_t index,
                            size_t len, int base,
                            const Fixed<31> w) {
      checkCapacity(w);

      if (len <= 8) {
         BitString<8> * f = (BitString<8>*)s;
         f += index;
         PutBits<1>::toBit(*f, len, w, base, *sink);
      } else if (len <= 15) {
         BitString<16> * f = (BitString<16>*)s;
         f += index;
         PutBits<2>::toBit(*f, len, w, base, *sink);
      } else if (len <= 31) {
         BitString<32> * f = (BitString<32>*)s;
         f += index;
         PutBits<4>::toBit(*f, len, w, base, *sink);
      } else if (len <= 63) {
         BitString<64> * f = (BitString<64>*)s;
         f += index;
         PutBits<8>::toBit(*f, len, w, base, *sink);
      } else {
         Log::error("unsupported length of fixed B-format (len=%zu)", len);
         throw theInternalDationSignal;
      }
   }
#endif

   void IOFormats::toT(const Clock f,
                       const Fixed<31> w,
                       const Fixed<31> d) {
      checkCapacity(w);
      PutClock::toT(f, w, d, *sink);
   }

   void IOFormats::fromT(Clock & f,
                         const Fixed<31> w,
                         const Fixed<31> d) {
      checkCapacity(w);
      GetClock::fromT(f, w, d, *source);
   }

   void IOFormats::toD(const Duration f,
                       const Fixed<31> w,
                       const Fixed<31> d) {
      checkCapacity(w);
      PutDuration::toD(f, w, d, *sink);
   }

   void IOFormats::fromD(Duration & f,
                         const Fixed<31> w,
                         const Fixed<31> d) {
      checkCapacity(w);
      GetDuration::fromD(f, w, d, *source);
   }


   int IOFormats::putDataFormat(TaskCommon * me, IODataEntry * dataEntry,
           size_t index, size_t loopOffset,IOFormatEntry * fmtEntry) {
      int returnValue = 0;

      switch (dataEntry->dataType.baseType) {
         default:
            printf("put unsupported baseType %d\n",
                   dataEntry->dataType.baseType);
            printf("fmt entry: format=%d data=%p type=%d width=%d, datasize=%zu\n",
                fmtEntry->format,
                dataEntry->dataPtr.inData,
                dataEntry->dataType.baseType, 
                dataEntry->dataType.dataWidth, 
                *dataEntry->numberOfElements);
          break;

      case IODataEntry::CHAR:  
         if (fmtEntry->format == IOFormatEntry::A) {
            toA((char*)(dataEntry->dataPtr.inData)+loopOffset,
                dataEntry->dataType.dataWidth,
                (Fixed<31>)(dataEntry->dataType.dataWidth));
         } else if (fmtEntry->format == IOFormatEntry::Aw) {
           toA((char*)(dataEntry->dataPtr.inData)+loopOffset,
               dataEntry->dataType.dataWidth,
               *fmtEntry->fp1.constF31Ptr);
         } else {
            Log::error("type mismatch in A format");
            throw theDationDatatypeSignal;
         }
         break;

      case IODataEntry::FIXED:
         if (fmtEntry->format == IOFormatEntry::Fw) {
            toFixedF((char*)(dataEntry->dataPtr.inData)+loopOffset, index,
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr);
         } else if ( fmtEntry->format == IOFormatEntry::Fwd) {
            toFixedF((char*)(dataEntry->dataPtr.inData)+loopOffset, index,
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr,
                  *fmtEntry->fp2.constF31Ptr);
         } else {
            Log::error("type mismatch in F format");
            throw theDationDatatypeSignal;
         }
         break;


      case IODataEntry::FLOAT:
         if (fmtEntry->format == IOFormatEntry::Fw) {
            toFloatF((char*)(dataEntry->dataPtr.inData)+loopOffset, index ,
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr);
         } else if ( fmtEntry->format == IOFormatEntry::Fwd) {
            toFloatF((char*)(dataEntry->dataPtr.inData)+loopOffset, index, 
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr,
                  *fmtEntry->fp2.constF31Ptr);
         } else {
            Log::error("type mismatch in F format");
            throw theDationDatatypeSignal;
         }
         break;

      case IODataEntry::BIT: 
         // treat B1, B2, B3, B4, B1w, B2w, B3w, B4w 
         {
             int base, length;
             Fixed<31> width;
         switch (fmtEntry->format) {
            default:
               Log::error("type mismatch in B format");
               throw theDationDatatypeSignal;
            case IOFormatEntry::B1:
               base = 1;
               length= dataEntry->dataType.dataWidth;
               width = (Fixed<31>)(length);
               break;
            case IOFormatEntry::B1w:
               base = 1;
               length= dataEntry->dataType.dataWidth;
               width = (Fixed<31>)(dataEntry->dataType.dataWidth);
               break;
         }
           
         toBit((char*)(dataEntry->dataPtr.inData)+loopOffset, index,
               length, base, width);
         }
         break;


      case IODataEntry::InduceData:
         Signal::throwSignalByRst(fmtEntry->fp1.intValue);
         break;
#if 0

      case IOJob::B1:
      case IOJob::B2:
      case IOJob::B3:
      case IOJob::B4:
#endif
      }

      return returnValue;

   }

   int IOFormats::getDataFormat(TaskCommon * me, IODataEntry * dataEntry,
           size_t index, size_t loopOffset, IOFormatEntry * fmtEntry) {
      int returnValue = 0;

      switch (dataEntry->dataType.baseType) {
         default:
            printf("get unsupported format %d\n", fmtEntry->format);
            printf("fmt entry: format=%d data=%p type=%d width=%d, datasize=%zu\n",
                fmtEntry->format,
                dataEntry->dataPtr.inData,
                dataEntry->dataType.baseType, 
                dataEntry->dataType.dataWidth, 
                *dataEntry->numberOfElements);
          break;

      case IODataEntry::CHAR:  
         if (fmtEntry->format == IOFormatEntry::A) {
            fromA((char*)(dataEntry->dataPtr.inData)+loopOffset,
                dataEntry->dataType.dataWidth,
                (Fixed<31>)(dataEntry->dataType.dataWidth));
         } else if (fmtEntry->format == IOFormatEntry::Aw) {
           fromA((char*)(dataEntry->dataPtr.inData)+loopOffset,
               dataEntry->dataType.dataWidth,
               *fmtEntry->fp1.constF31Ptr);
         } else {
            Log::error("type mismatch in A format");
            throw theDationDatatypeSignal;
         }
         break;

      case IODataEntry::FIXED:
         if (fmtEntry->format == IOFormatEntry::Fw) {
            fromFixedF((char*)(dataEntry->dataPtr.inData)+loopOffset, index,
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr);
         } else if ( fmtEntry->format == IOFormatEntry::Fwd) {
            fromFixedF((char*)(dataEntry->dataPtr.inData)+loopOffset, index,
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr,
                  *fmtEntry->fp2.constF31Ptr);
         } else {
            Log::error("type mismatch in F format");
            throw theDationDatatypeSignal;
         }
         break;


      case IODataEntry::FLOAT:
         if (fmtEntry->format == IOFormatEntry::Fw) {
            fromFloatF((char*)(dataEntry->dataPtr.inData)+loopOffset, index ,
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr);
         } else if ( fmtEntry->format == IOFormatEntry::Fwd) {
            fromFloatF((char*)(dataEntry->dataPtr.inData)+loopOffset, index, 
                  dataEntry->dataType.dataWidth,
                  *fmtEntry->fp1.constF31Ptr,
                  *fmtEntry->fp2.constF31Ptr);
         } else {
            Log::error("type mismatch in F format");
            throw theDationDatatypeSignal;
         }
         break;

      case IODataEntry::InduceData:
         Signal::throwSignalByRst(fmtEntry->fp1.intValue);
         break;
#if 0

      case IOJob::B1:
      case IOJob::B2:
      case IOJob::B3:
      case IOJob::B4:
#endif
      }

      return returnValue;

   }
}


