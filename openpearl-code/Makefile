# super makefile for SmallPearl
# 
-include configuration/.config

.PHONY: install runtime compiler help build all menuconfig

help:
	@echo "*************************************"
	@echo "master make for SmallPEARL"
	@echo "make build:   build compiler and runtime"
	@echo "make install: copy build stuff to /usr/local/... (must be root)"
	@echo "make all:     build+install"
	@echo "make clean:   clean all parts"
	@echo "*************************************"

all:	build install

build: runtime compiler

runtime:
	(cd runtime; make all; )

compiler:
	(cd compiler/src; make all; )

install: configuration/include/autoconf.h 
	(cd runtime; make clean; )
	(cd compiler/src; make clean; )
	(cd runtime; make install; )
	(cd compiler/src; make install; )
	runtime/includeComposer/ic  <prl.inc >prl
	# is created by includeComposer but not needed here
	@rm -f sysincs.h
	@cp prl $(CONFIG_INSTALL_Target)/bin
	@chmod a+x $(CONFIG_INSTALL_Target)/bin/prl

# create rule to run menuconfig, if no configuration/include/autoconf.h exists
#ifeq ($(wildcard "configuration/include/autoconf.h")),)


.PHONY: configuration/include/autoconf.h
configuration/include/autoconf.h: 
ifneq ($(wildcard ./configuration/include/autoconf.*),./configuration/include/autoconf.h)
	(cd configuration; make include/autoconf.h; cd ..)
	make menuconfig
endif

clean:
	(cd runtime; make clean; )
	(cd compiler/src; make clean; )
	(cd testsuite; make clean; )
	(cd language_report; make clean; )
	(cd configuration; make clean;)
	rm -f prl

menuconfig:
	(cd configuration; make menuconfig; ./scripts/kconfig/conf -s fm/main.fm; cd .. )
