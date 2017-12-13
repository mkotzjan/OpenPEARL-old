/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

#ifndef IOJOB_INCLUDED
#define IOJOB_INCLUDED

/**
\file

\brief classes for the definition of the structures for IOJobs
*/

#include "Fixed.h"
#include "TaskCommon.h"
#include "SystemDationNBSink.h"

namespace pearlrt {
   class IODataEntry;
   class IOFormatEntry;

   /**
   \addtogroup io_common_helper
   @{
   */

   /**
   \brief list of data element for one io statement 

   the i/o is treated by data and format lists, which contain all statement
   concerning one PEARL I/O statement like READ, WRITE, PUT, GET, TAKE or SEND.

   A job consists of a vector of data entries and a vector of format entries.
   If more data entries than format entruies exist, the treatment of 
   vector of format entries is restarted.
   
   The required access to the PEARL data elements is realized via void
   pointers.

   For details about defining an IODataEntry and IOFormatEntry 
   please refer to their documentation.
   Expressions in the datalist must be evaluated before starting the io job.
   For each expression a result variable must be introduced.

   STRUCTs must be rolled out by the compiler downto the simple types.

   Example:<br>
    x and width are FIXED(15) and y is a FLOAT(53)
    
    \code
    PUT 'X=',x, 'Y=', y+1 TO so BY A,F(5), X(width),A(4), E(13,6),SKIP;
    \endcode

    The code for the data list and result variables should look like:
    \code
    Fixed<15> resultOfExpr1;

    IODataEntry dataEntries[] = {
       { /* index 0 */
          .datatype={CHAR,2},
          .dataPtr{.outData=&CONST_CHAR_1234}
       },
       { /* index 1 */
          .datatype={IODataEntry::FIXED,15},
          .dataPtr{.outData=&_x}
       },
       { /* index 2 */
          .datatype={IODataEntry::CHAR,2},
          .dataPtr{.outData=&CONST_CHAR_4567}
       },
       { /* index 3 */
          .datatype={IODataEntry::FLOAT,53},
          .dataPtr{.outData=&resultOfExpr1}
       },
    };
    try {
       resultOfExpr1 = _y + CONST_FIXED_POS_1_31;
    } catch (Signal & s) {
       // modify data entry to throw the signal
       dataEntries[3].dataType.baseType = IODataEntry::InduceData;
       dataEntries[3].dataType.dataWidth = s.whichRST(); 
    }

    IODataList dataList={sizeof(dataEntries)/sizeof(dataEntries[0]),
                         dataEntries);
                      
   \endcode
   */
   class IODataList {
   public:
      /** the number of entries in the complete job
      */
      size_t nbrOfEntries;

      /**
      the individial entries will be passed as an array.
      */
      IODataEntry * entry;
   };

   /**
   \brief list of format elements for one io statement 

   the i/o is treated by data and format lists, which contain all statement
   concerning one PEARL I/O statement like READ,WRITE,PUT,GET,TAKE or SEND.

   A job consists of a vector of data entries and a vector of format entries.
   If more data entries than format entries exist, the treatment of 
   vector of format entries is restarted.
   
   The required access to the PEARL data elements is realized via void
   pointers.

   For details about defining an IODataEntry and IOFormatEntry 
   please refer to their documentation.

   Example:
    Assume x and width as FIXED(15), and y as FLOAT(53)
 
    \code
    PUT 'X=',x, 'Y=', y+1 TO so BY A,F(5), X(width),A(4), E(13,6),SKIP;
    \endcode

    The code for the format list and result  variables should look like:
    \code
    Fixed<31> resultOfExpr2;

    IOFormatEntry formatEntries[] = {
       { /* index 0 */
          .format=IOFormatEntry::A,
       },
       { /* index 1 */
          .format=IOFormatEntry::Fw,
          .fp1={.outParam=&CONST_FIXED_POS_5_31,
       },
       { /* index 2 */
          .format=IOFormatEntry::X,
          .fp1={.outParam=&resultOfExpr2},
       },
       { /* index 3 */
          .format=IOFormatEntry::Aw,
          .fp1={.outParam=&CONST_FIXED_POS_4_31,
       },
       { /* index 4 */
          .format=IOFormatEntry::E2wd,
          .fp1={.outParam=&CONST_FIXED_POS_13_31,
          .fp2={.outParam=&CONST_FIXED_POS_6_31,
       },
       { /* index 5 */
          .format=IOFormatEntry::SKIP,
          .fp1={.outParam=&CONST_FIXD_POS_1_31},
       },
    };

    try {
      resultOfExpr2 = width;
    } catch (Signal & s) {
       // modify format entry to throw the signal
       formatEntries[5].format = IOFormatEntry::InduceFormat;
       formatEntries[5].intValue = s.whichRST(); 
    }
       
    \endcode

   */
   class IOFormatList {
   public:
      /** the number of entries in the complete job
      */
      size_t nbrOfEntries;

      /**
      the individial entries will be passed as an array.
      */
      IOFormatEntry * entry;
   };

   /**
   \brief specification of a data element inside of an io job.

   Example:
   reading a value into the FLOAT(53) variable 
   \code
   {
      .dataType={IODataEntry::FLOAT,53},
      .dataPtr.in = _x, 
      .numberOfElements = 1,
   }
   \endcode

   */
   class IODataEntry {
   public:
      /**
      the identifiers of the base type of an IODataEntry

      The type information is completed with the dataWidth element
      for the templated simple data types.
      */
      enum IODataType  {
          CHAR, ///< CHAR types
          FLOAT, ///< FLOAT types
          FIXED, ///< FIXED types
          BIT,   ///< BIT types
          CLOCK, ///< CLOCK types
          DURATION, ///< DURATION types
	  /** a virtual type, which is used to inject an
              PEARL signal, if the evaluation of an expression
              for data transfers was not possible.

              fp1.intValue must contain the signal number 
          */
	  InduceData,

          /** a virtual type for the repetition of format elements
              like in (3)((5)(F(4), X(3)),SKIP) 
              which represents 3 lines with 5 fixed values and 3 spaces 
              
              fp1.intValue must contain the number of enveloped
                   format elements (this entry is not counted)<br>
              fp2.intValue must contain the number of repetitions
          */
          LoopStart,
      };

      /** the number of entries in the complete job
      */
      /** the type of the data element
      */
      struct {
           /** the base type is one of the enum values CHAR, BIT,.., CLOCK*/
           IODataType baseType:8;    
           /** this is the number of bits for the types which
           exists in several widths, otherwise may be unitialized */ 
           unsigned int dataWidth:16;
      } dataType;

      /**
      Pointer to the data which should be transferred from/to the peripheral.

      Depending on the direction, ether the component inData for input
      or outData schould be used. 
      Input data need a non constant pointer. For output data, the 
      const void pointer should be used to be compatible with real constants
      */
      union {
         void * inData;		///< non constant pointer for input
         const void * outData;     ///< constant pointer for output
      } dataPtr;                   ///< the data pointer component

      /**
      number of elements

      For array slices, we need the number of data elements of the slice.
      For scalar values this item must be 1.
      */
      size_t numberOfElements;
   };

   /**
   \brief specification of a format element inside of an io job.

   Example:
   reading a value into the FLOAT(53) variable x with the
   format F(13,6)
   \code 
   {
      .format=IOFormatEntry::F,
      .fp1.outParam=&CONST_FIXED31_13,
      .fp2.outParam=&CONST_FIXED31_6
   }
   \endcode
   */
   class IOFormatEntry {
     public:
      /**
      the format identifiers of an IOFormatEntry
      */
      enum IOFormat  {
            /** the A-format without width specification */
            A,
            /** the A-format with width specification<br>
            fp1.outParam must refer the field width */
            Aw,

            /** F-format for FIXED and FLOAT types width width<br>
            fp1.outParam must refer the field width */
            Fw,

            /** F-format for FIXED and FLOAT types with
                          width and precision<br>
            fp1.outParam must refer the field width <br>
            fp2.outParam must refer the number of decimals
            */
            Fwd,

            /** E or E2-format for FLOAT types with
                          width and precision
            fp1.outParam must refer the field width <br>
            fp2.outParam must refer the number of decimals
            */

            E2wd,
            /** E3-format for FLOAT types with
                          width and precision
            fp1.outParam must refer the field width <br>
            fp2.outParam must refer the number of decimals
            */

            E2wds,
            /** E or E2-format for FLOAT types with
                          width, precision and significance
            fp1.outParam must refer the field width <br>
            fp2.outParam must refer the number of decimals<br>
            fp3.outParam must refer the number of significant digits
            */

            E3wd,
            /** E3-format for FLOAT types with
                          width, precision and significance
                          width and precision
            fp1.outParam must refer the field width <br>
            fp2.outParam must refer the number of decimals<br>
            fp3.outParam must refer the number of significant digits
            */
            E3wds,
            
            /** the B or B1 format without width specification */
            B1,
            /** the B2 format without width specification */
            B2,
            /** the B3 format without width specification */
            B3,
            /** the B4 format without width specification */
            B4,
            /** the B or B1 format width width specification <br>
            fp1.outParam must refer the field width */
            B1w,
            /** the B2 format width width specification<br>
            fp1.outParam must refer the field width  */
            B2w,
            /** the B3 format width width specification<br>
            fp1.outParam must refer the field width  */
            B3w,
            /** the B4 format width width specification<br>
            fp1.outParam must refer the field width  */
            B4w,

            /** T format with width <br>
            fp1.outParam must refer the field width */
            Tw,

            /** T format with width and decimals<br>
            fp1.outParam must refer the field width<br>
            fp2.outParam must refer the number of decimals  */
            Twd,

            /** D format with width<br>
            fp1.outParam must refer the field width  */
            Dw,

            /** D format with width and decimals<br>
            fp1.outParam must refer the field width <br>
            fp2.outParam must refer the number of decimals */
            Dwd,

            /** LIST format will use the default format 
                depending on the data type */
            LIST,


            /**
            for treatment of the io-entries we must differ
            between data entries and formating entries.
            Data entries will be treated inside the IOFormat class
            Format entries will be treated inside the
            UserDationNB class.
            RST fits better to the format entries.
            This element ist never used by the applications
            */
            IsPositioning,

            /** 
            the RST format entry 

    	    fp1.inData must refer the rst variable<br>
            fp2.intValue contains the dataSize
            */
            RST,

            /** X positioning<br> 
            fp1.outParam must refer the field width  
            */
            X,

            /** SKIP positioning <br>
            fp1.outParam must refer the number of lines to terminate */
            SKIP,

            /** PAGE positioning <br>
            fp1.outParam must refer the number of pages to skip */
            PAGE,

            /** a virtual format entry, which is used to inject a PEARL 
                signal, if the evaluation of an expression of a
                format item failed

              fp1.intValue must contain the signal number 
            */
            InduceFormat
      };

      /**
      the requested io format.
      */
      IOFormat format;

      /**
      The format elements may contain 1 to 3 parameters.

      Unused parameters may be left uninitialized.

      Format elements like RST or SOP need a parameter to store data
      from the dation status. Other parameters are used read only by
      the formatting like the width of an F-format
      */
      union FormatParameter{
           /** pointer to a FIXED(31) value to receive results like in SOP */
           Fixed<31> * f31Ptr;
           /** pointer to a FIXED(31) value to specify width or precision */
           Fixed<31> const * constF31Ptr;
           /** generic pointer used only in RST format for the RST variable */
           void * voidPtr;
           /** an int value used only in InduceFormat item to specify the 
               signal number  */
           int intValue;
      } fp1,  ///< first format parameter
        fp2,  ///< second format parameter
        fp3;  ///< third format parameter
   };
   /* @} */
}
#endif

