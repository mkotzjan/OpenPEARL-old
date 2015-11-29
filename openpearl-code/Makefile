# super makefile for SmallPearl
# 
include configuration/.config

.PHONY: runtime compiler help build all menuconfig

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

install: clean
	(cd runtime; make install; )
	(cd compiler/src; make install; )
	runtime/includeComposer/ic  <prl.inc >prl
	rm -f sysincs.h		# are created by includeComposer but not needed here
	cp prl $(CONFIG_INSTALL_Target)/bin

clean:
	(cd runtime; make clean; )
	(cd compiler/src; make clean; )
	(cd testsuite; make clean; )
	(cd language_report; make clean; )
	(cd configuration; make clean;)
	rm -f prl
	
menuconfig:
	(cd configuration; make menuconfig; ./scripts/kconfig/conf -s fm/main.fm; cd .. )
