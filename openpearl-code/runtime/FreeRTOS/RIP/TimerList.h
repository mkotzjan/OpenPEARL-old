/*
 [The "BSD license"]
 Copyright (c) 2013-2014 Florian Mahlecke
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
 * TimerList.h
 *
 *  Created on: 30.05.2014
 *      Author: Florian Mahlecke
 */

#ifndef TIMERLIST_H_
#define TIMERLIST_H_


#include "Task.h"
#include "Timer.h"
#include <set>


namespace pearlrt {


class CompareTimer {

	public:
	bool operator()(const Timer* left,const Timer* right);

};


class TimerList {

	private:
	TimerList(){}									///< private so that it can not be called (singelton pattern)
	TimerList(TimerList const&){}	///< copy constructor is private
	TimerList& operator=(TimerList const&);	///< assignment operator is private


	public:
	std::set<Timer*,CompareTimer> tl;			///< binary search tree (STL set)
	static TimerList& Instance();
	void addTimerToList(Timer* timer);
	void removeTimerFromList(Timer *timer);
	static Timer* getTimer();
	static int getSize();
	void setAlaram();

#ifdef DEBUGMODE
	void printList();
#endif
};





// end of namespace
}


#endif /* TIMERLIST_H_ */
