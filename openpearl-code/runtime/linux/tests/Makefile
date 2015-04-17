#
# [The "BSD license"]
# Copyright (c) 2012-2013 Rainer Mueller
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
# pearl runtime system tests
#CXXFLAGS = -O2 -Wall -I .. -I ../.. -g -fstack-protector

CXXFLAGS = $(CXXFLAGS4SUB)
CXXFLAGS += -I ../../common -I ..
LDFLAGS=$(LDFLAGS4SUB)

$(warning Info: in SUB CXXFLAGS=$(CXXFLAGS))

DEPENDFILE = .depend

TESTS +=  \
	DataTypeTests \
	log_test api  lot_of_tasks  \
	susp_cont_test \
	sched_demo \
	interruptTest \
	SampleBasicDationTests \
	sematest sematerminate  \
	signalTest\
	DationTests \
	DationSuspendTerminateTests
	

TEST_SRCS +=  log_test.cc \
	../../common/tests/sematest.cc ../../common/tests/sematerminate.cc\
	../../common/tests/interruptTest.cc \
	../../common/tests/SampleBasicDationTests.cc

TEST_DATION_SRCS += gtest_main.cc myTests.cc  \
	DiscTests.cc PipeTests.cc \
	UserDationTests.cc\
	DationRWTests.cc DationPGTests.cc


GTEST_SRCS += gtest_main.cc \
	PrioTests.cc PrioMapperTests.cc

include ../../common/tests/Files

TEST_CHECK_FILES += $(TEST_SRCS) $(GTEST_SRCS) $(TEST_DATION_SRCS)

GTEST_OBJS =$(GTEST_SRCS:.cc=.o)

DATION_OBJS =$(TEST_DATION_SRCS:.cc=.o)

.PHONY: clean
.PHONY: depend
.PHONY: checkformat


all: setup depend $(TESTS) 

setup:
ifeq ($(CXXFLAGS4SUB),) 
	   @echo " ***** do the make from one level up" 
	   @exit 2 
endif 

signalTest: signalTest.o ../plib.a
	$(CXX) $^ $(LDFLAGS) -o signalTest -pthread

interruptTest: ../../common/tests/interruptTest.o ../plib.a
	$(CXX) $^ $(LDFLAGS) -o interruptTest -pthread
		
SampleBasicDationTests: ../../common/tests/SampleBasicDationTests.o ../plib.a
	$(CXX) $^ $(LDFLAGS) -o SampleBasicDationTests -pthread
		
api: api.o ../plib.a
	$(CXX) $^ $(LDFLAGS) -o api -pthread
		
sched_demo: ../../common/tests/sched_demo.o ../../common/tests/round_robin_test.o ../plib.a
	$(CXX) $^ $(LDFLAGS) -o sched_demo -pthread

lot_of_tasks: lot_of_tasks.o ../plib.a
	$(CXX) $^ $(LDFLAGS) -o lot_of_tasks -pthread


DataTypeTests: $(GTEST_OBJS) ../plib.a
	$(CXX) $(CXXFLAGS) $^ \
		/usr/local/lib/libgtest.a $(LDFLAGS) -o $@ -pthread

DationTests: $(DATION_OBJS) ../plib.a
	$(CXX) $(CXXFLAGS) $^ \
		/usr/local/lib/libgtest.a $(LDFLAGS) -o $@ -pthread

log_test: log_test.o ../plib.a
	$(CXX) $^  -o log_test -pthread

susp_cont_test: ../../common/tests/susp_cont_test.o ../plib.a
	$(CXX) $^  -o susp_cont_test  $(LDFLAGS) -pthread

sematest: ../../common/tests/sematest.o ../plib.a
	$(CXX) $^  -o $@ $(LDFLAGS)  -pthread

sematerminate: ../../common/tests/sematerminate.o ../plib.a
	$(CXX) $^  -o $@ $(LDFLAGS)  -pthread

DationSuspendTerminateTests: DationSuspendTerminateTests.o ../plib.a
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@ -pthread

clean: 
	rm -f *.o
	rm -f ../../common/tests/*.o
	rm -f $(GTEST_OBJS) $(DATION_OBJS)
	rm -f *.new
	rm -f *.diff
	rm -f $(TESTS)
	rm -f pearl_log.txt 
	rm -f *.log 
	rm -rf latex
	rm -rf html
	rm -rf core.*
	rm -rf .depend

checkformat:
	@$(foreach f,$(TEST_CHECK_FILES),../../codestyle/check.sh $(f);)


depend: $(TEST_CHECK_FILES)
	$(CC) $(CXXFLAGS) -MM $(TEST_CHECK_FILES) > $(DEPENDFILE)

-include .depend
