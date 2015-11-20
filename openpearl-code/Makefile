# super makefile for SmallPearl
# 
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

install: clean build
	(cd runtime; make install; )
	(cd compiler/src; make install; )
	cp prl_bin /usr/local/bin

clean:
	(cd runtime; make clean; )
	(cd compiler/src; make clean; )
	(cd testsuite; make clean; )
	
menuconfig:
	(cd configuration; make menuconfig; ./scripts/kconfig/conf -s fm/main.fm; cd .. )
