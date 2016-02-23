#
# [The "BSD license"]
# Copyright (c) 2012-2015 Rainer Mueller
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
# pearl runtime system tests for lpc1768 plattform

# overwiew of liste:
# targets - list of test application to build
# GTEST_SRC        - list of module to be added to the "mini" google testframe 
#                    for data type tests
# GTEST_DATION_SRC - list of module to be added to the "mini" google testframe 
#                    for dation tests
# TEST_SRCS        - list of modules for standalone tests

PLIB = OpenPEARLlpc1768_board.o libOpenPEARLlpc1768.a

targets +=  \
	tests/DataTypeTests1 \
	tests/DataTypeTests2 \
	tests/susp_cont_test \
	tests/sched_demo \
	tests/SampleBasicDationTests \
	tests/sematest tests/sematerminate  \
	tests/signalTest \
	tests/clockTest \
	tests/interruptTest \
	tests/gpioInterrupt \
	tests/log_test \
	tests/DationTests \
#	tests/DationSuspendTerminateTests 
	
INCS += ../ugtest

GTEST_SRCS = ../ugtest/simpleTests.cc 
GTEST_DATION_SRCS = ../ugtest/simpleTests.cc 

GTEST_SRCS += tests/PrioTests.cc
GTEST_SRCS += tests/PrioMapperTests.cc


GTEST_DATION_SRCS +=   \
	tests/UartTests.cc
#  tests/DiscTests.cc tests/PipeTests.cc \
#  tests/UserDationNBTests.cc\
#  tests/StdStreamTests.cc \
#  tests/DationRWTests.cc tests/DationPGTests.cc


# ----------------------------------------
# include test application code from the common stuff
include ../common/tests/Files
#TEST_SRCS += $(addprefix ../common/tests/,$(CXX_COMMON_TESTS))
GTEST_SRCS1 = $(addprefix ../common/tests/,$(CXX_COMMON_TESTS1))
GTEST_SRCS2 = $(addprefix ../common/tests/,$(CXX_COMMON_TESTS2))
# ----------------------------------------
GTEST_OBJS =$(GTEST_SRCS:.cc=.o)
GTEST_OBJS1 =$(GTEST_SRCS1:.cc=.o)
GTEST_OBJS2 =$(GTEST_SRCS2:.cc=.o)

GTEST_DATION_OBJS =$(GTEST_DATION_SRCS:.cc=.o)

# ----------------------------------------
# define rules to build the targets
#  due to problems von implicit rules, the link command is 
#  given as explicit rule

tests/gpioInterrupt: tests/gpioInterrupt.o $(PLIB)
	$(LINK)

tests/signalTest: tests/signalTest.o $(PLIB)
	$(LINK)

tests/log_test: ../common/tests/log_test.o $(PLIB)
	$(LINK)

tests/interruptTest: ../common/tests/interruptTest.o $(PLIB)
	$(LINK)

tests/SampleBasicDationTests: ../common/tests/SampleBasicDationTests.o $(PLIB)
	$(LINK)

tests/api: tests/api.o $(PLIB)
	$(LINK)

tests/sched_demo: ../common/tests/sched_demo.o \
	../common/tests/round_robin_test.o $(PLIB)
	$(LINK)

tests/susp_cont_test: ../common/tests/susp_cont_test.o $(PLIB)
	$(LINK)

tests/lot_of_tasks: tests/lot_of_tasks.o $(PLIB)
	$(LINK)

tests/DataTypeTests1: $(GTEST_OBJS) $(GTEST_OBJS1) $(PLIB)
	$(LINK)

tests/DataTypeTests2: $(GTEST_OBJS) $(GTEST_OBJS2) $(PLIB)
	$(LINK)

tests/DationTests: $(GTEST_DATION_OBJS) $(PLIB) 
	$(LINK)

tests/sematest: ../common/tests/sematest.o $(PLIB)
	$(LINK)

tests/sematerminate: ../common/tests/sematerminate.o $(PLIB)
	$(LINK)

tests/DationSuspendTerminateTests: tests/DationSuspendTerminateTests.o $(PLIB)
	$(LINK)

tests/x: tests/x.o $(PLIB)
	$(LINK)

tests/clockTest: tests/clockTest.o $(PLIB)
	$(LINK)

tests/Post: tests/Post.o $(PLIB)
	$(LINK)

