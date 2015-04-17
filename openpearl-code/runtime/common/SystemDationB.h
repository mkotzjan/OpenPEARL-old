/*
 [The "BSD license"]
 Copyright (c) 2012-2013 Holger Koelle
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

#ifndef SYSTEMDATIONB_INCLUDED
#define SYSTEMDATIONB_INCLUDED

#include "Dation.h"
#include "SystemDation.h"
#include "Fixed.h"

/**
\file

\brief systemdation interface for BASIC dations

*/

namespace pearlrt{

/** 
 defines the interfaces to communicate with a user defined (BASIC) dation.
 BASIC dations don't support userdations, they will be treated like
 systemdations.

 In future, there should be a signature to distinguish between BASIC
 and NON-BASIC dations. BASIC dations are opened directly by the application.
 They need no user defined dation. 
*/
class SystemDationB: public SystemDation {

/**
\file

\brief systemdation interface for BASIC dations
       
       defines the interfaces to communicate with a user defined (BASIC) dation.
       BASIC dations don't support userdations, they will be treated like systemdations.

       In future, there should be a signature to distinguish between BASIC and NON-BASIC
       dations. BASIC dations are opened directly by the application.
       They need no user defined dation. 
*/
protected:

public:
    /**
    Open the device/file.

    To support multiple files on a system dation, the openDation must be
    realized as factory. The capacity is defined by the concrete system
    dation (e.g. 20 for files, or 1 for a serial line).

    \param idfValue as C-string containing the value of the IDF-field
    \param openParam a bit map containing the required operation

    \returns reference to the new object

    \throws * may throw exceptions in case of problems in execution
    */
    virtual SystemDationB* dationOpen(const char * idfValue,
                                       int openParam) = 0;
#ifdef XXXXXXXXXXXXXXX
        /**
        dation open method

        The method may throw exceptions, if no rstValue is given.

        \param openParam open parameters as bit-ored-value
        \param idf  pointer to idf-string if given
        \param rstValue  pointer to rst error variable if given
        */
        template<size_t S>
        void dationOpen(int openParam,
                        Character<S>* idf=0,
                         Fixed<31>* rstValue=0) {
	    Log::error("dationOpen for BASIC dation not implemented");
            throw theInternalDationSignal;
        }
#endif
        /**
        dation close method

        The method may throw exceptions, if no rstValue is given.

        \param parameters open parameters as bit-ored-value
	*/
	virtual void dationClose(int parameters=0)=0;

        /**
        dation write method

        The method may throw exceptions.

        \param start pointer to the starting adress of the data be be written
        \param size number of bytes to be written
        */
	virtual void dationWrite(void * start, size_t size)=0;

        /**
        dation read method

        The method may throw exceptions.

        \param start pointer to the starting adress of the data be be read
        \param size number of bytes to be read
        */
	virtual void dationRead(void * start, size_t size)=0;
};
}
#endif
