#/*
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
#*/

# read system configarion setting from menuconfig
include ../../configuration/.config

# pearl runtime system for linux
# this makefile is only called from the superior makefile
# if CONFIG_LINUX is set to y

INCLUDES = -I . -I ../common
CXXFLAGS = -Wall -g  -fstack-protector
CXXFLAGS += -pthread
CXXFLAGS += -fno-non-call-exceptions
CXXFLAGS += $(INCLUDES)
LDFLAGS = -lpthread -lrt 

#LINK.o = @echo link $<; $(LINK.cc)
LINK.o = $(LINK.cc)

# shortcut for link command, since the implicit link rules of 
# make produces problems if the dependencies reside in different
# targets. Thus an explicit rule is added for the targets (especially
# in the tests/ directory
LINK = @echo link $@; $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@

# make silent compilations 
COMPILE.cc = @echo compile $<; $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

# helper commands include composer, deleted white spaces
IC_DIR = ../includeComposer
IC = $(IC_DIR)/ic
DWS = $(IC_DIR)/dws

DEPENDFILE = .depend

phony = _all
_all: all

SRCANDH = \
	TaskTimer.cc \
	Task.cc \
	Mutex.cc PrioMapper.cc \
	CSema.cc \
	Pipe.cc Disc.cc StdStream.cc \
	UnixSignal.cc 

SRCONLY =os.cc \
	TaskMonitor.cc \
	Log.cc  

# ---------------------------------------
# import lists of common files for all implementations
# and add prefix of directory common to file lists
include ../common/Files.common
CXX_COMMON := $(addprefix ../common/,$(CXX_COMMON))
HDR_COMMON := $(addprefix ../common/,$(HDR_COMMON))

# ---------------------------------------
# list of file for checkformat
CHECK_FILES = $(sort $(SRC) $(SRCONLY) $(SRC_H) \
		     $(CXX_COMMON) $(HDR_COMMON) 


# -----------------------------------
# add driver specific stuff to file lists

ifeq ($(CONFIG_LINUX_Octopus),y) 
   SRCANDH += drivers/Octopus.cc
   SRCANDH += drivers/OctopusDigitalOut.cc
   SRCANDH += drivers/OctopusDigitalIn.cc
   CHECK_FILES += drivers/Octopus.cc drivers/Octopus.h
   CHECK_FILES += drivers/OctopusDigitalOut.cc drivers/OctopusDigitalOut.h
   CHECK_FILES += drivers/OctopusDigitalIn.cc drivers/OctopusDigitalIn.h
   INCLUDES += -I ./drivers 
   INCLUDES += -I ../drivers 
   LDFLAGS +=  -lusb -loctopus
endif

targets := libOpenPEARLlinux.a

ifeq ($(CONFIG_LINUX_tests),y)
  include tests/Makefile.inc
  CHECK_FILES += $(GTEST_SRC) $(GTEST_DATION_SRC) $(TEST_SRC)
endif

# ------------------------------------
CHECK_FILES = $(sort $(CHECK_FILES))
SRC = $(SRCANDH) $(SRCONLY) $(CXX_COMMON)
#$(warning src = $(SRC))

PLIB_OBJ = $(SRC:%.cc=%.o)
#$(warning plibobj = $(PLIB_OBJ))

ALL_SRCS = $(SRC) $(TEST_SRCS) $(TEST_DATION_SRCS) $(GTEST_SRCS)
#$(warning allsrc = $(ALL_SRCS))


SRC_H = $(SRCANDH:%.cc=%.h) PearlIncludes.h  $(HDR_COMMON)

OBJS = $(ALL_SRCS:%.cc=%.o)

# ------------------------------

phony += all
all: depend  $(targets)

phony += help
help:
	@echo "-----------------"
	@echo "make all         : build the pearl library lubOpenPEARLlinux.a"
	@echo "make clean       : remove all generated files"
	@echo "make checkformat : apply code style checks"
	@echo "make doc         : generate Doxygen documentation in HTML"
	@echo "make install     : install to $(CONFIG_INSTALL_Target) in lib"
	@echo "                 : and prepare include file" 
	@echo "-----------------"
	@echo

phony += install
install: all
	rm -f pearl2.h
	$(IC) -c $(INCLUDES)  <PearlIncludes.h | $(DWS) >pearl2.h
	cat sysincs.h pearl2.h \
		>pearl.h
	cp libOpenPEARLlinux.a $(CONFIG_INSTALL_Target)/lib
	rm -f pearl2.h sysincs.h
	# ########
	echo '$(CXX) -DTARGET=1 $(CXXFLAGS) $(CFLAGS) -I $(CONFIG_INSTALL_Target)/include -lrt -pthread $(CONFIG_INSTALL_Target)/lib/libOpenPEARLlinux.a $$fn.cc -o $$fn' > cc_bin.inc
	# ##########
	echo './$$fn' >run_bin.inc

libOpenPEARLlinux.a: $(PLIB_OBJ)
	rm -f $@
	@echo build archive ... libOpenPEARL.a
	ar r $@  $(PLIB_OBJ) 

# -------------------------------------------------
# additional rules to enable make to build the autogenerated files
#
../common/Signals.hh: ../common/Signals.ods
	@echo create signal definitions from spreadsheet
	@(cd ../common; \
	  perl GenerateSignalDefinitions.pl GENERIC LINUX 2>signals.log;)

../common/Signals.hcc: ../common/Signals.ods
	(cd ../common; perl GenerateSignalDefinitions.pl GENERIC LINUX;)

../Signals.cc: ../Signals.hh ../Signals.hcc

#---------------------------------------------------------
phony += clean
clean: 
	@echo cleaning created files in various directories
	@rm -f $(OBJS)
	@rm -f pearl_log.txt
	@rm -f libOpenPEARLlinux.a
	@rm -rf latex
	@rm -rf html
	@rm -f .depend
	@rm -f ../common/tests/*.new
	@rm -f ../common/tests/*.diff
	@rm -f ../common/*.new
	@rm -f ../common/*.diff
	@rm -f *.new
	@rm -f *.diff
	@rm -f *.log
	@rm -f drivers/*.new
	@rm -f drivers/*.diff
	@rm -f core
	@rm -f ../common/Signals.hh ../common/Signals.hcc
	@rm -f ../common/signallist.tex 
	@rm -f ../common/signals.log
	@rm -f sysincs.h  pearl2.h pearl.h
	rm -f cc_bin.inc run_bin.inc

phony += checkformat
checkformat: 
	@$(foreach f,$(CHECK_FILES),../codestyle/check.sh $(f);)
	make -C tests checkformat

phony += doc
doc:
	doxygen doc_stuff/Doxyfile
#	(cd latex; make pdf; cd ..)

phony += depend
depend: $(ALL_SRCS) ../common/Signals.hh ../common/Signals.hcc
	@echo update dependencies ...
	@$(CC) -MM  $(INCLUDES) $(ALL_SRCS) > $(DEPENDFILE)

-include .depend

.PHONY: $(phony)
