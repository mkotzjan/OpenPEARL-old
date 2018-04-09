/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
 Copyright (c) 2014 Rainer Mueller
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

/*
 * GenericTask.h
 *
 *  Created on: 13.03.2014
 *      Author: Florian Mahlecke, Rainer Mueller
 */


/*********************************************************************//**
 * @file
 * @brief	OpenPEARL C/C++ macros
 **********************************************************************/

#ifndef GENERICTASK_H_
#define GENERICTASK_H_

#include "Task.h"
#include "Prio.h"
#include "BitString.h"


/**
  \brief	SPECIFY TASK macro

  generate class name with prefix C_ and leave the objects name
  without change, since the compiler uses the objects name for
  the tasking operations.
  The task class and object resides in the namespace of the application

  \param x the user defined name of the task
*/
#define SPCTASK(x) \
    class C_##x : public pearlrt::Task {				\
    public:								\
       C_##x(char *xx, pearlrt::Prio pp, pearlrt::BitString<1> ii):	\
		pearlrt::Task(xx,pp,ii){}; 				\
       void task(pearlrt::Task * me);				\
    }; 									\
    extern class C_##x x


/**
  \brief	DECLARE TASK macro

  generate the task object and the header of the task()-method
  the compiler adds the task()-methods body immediatelly after
  this (expanded) macro.
  For explanations on the decoration scheme see SPCTASK
  The task class and object resides in the namespace of the application

  \param x the user defined name of the task
  \param prio the user supplied priority of the task
  \param ismain a pearlrt::BitString<1> value to specify whether the task
                 has a MAIN attribute set
 */
#define DCLTASK(x, prio, ismain) 			\
 C_##x x __attribute__ ((section ("PEARL_APPLICATION"))) \
	 ((char*)#x,prio, ismain);	\
 void C_##x::task(pearlrt::Task * me)

#endif /* GENERICTASK_H_ */
