# super makefile for OpenPEARL
# 
USERID=$(shell id -u)

-include configuration/.config

.PHONY: install runtime compiler help build all menuconfig

help:
	@echo "*************************************"
	@echo "master make for OpenPEARL"
	@echo "make build:   build compiler and runtime"
	@echo "make prepare: create installation targets with public write access"
	@echo "              should be executed once for developers"
	@echo "              (must be root)"
	@echo "make install: build and install to the configured installation target"
	@echo "              if target is /usr/local/  you must be root"
	@echo "              after make prepare, this may be done as normal user"
	@echo "make defconfig: create default configuration setting"
	@echo "make menuconfig: edit configuration setting"
	@echo "make all:     install"
	@echo "make clean:   clean in all project parts"
	@echo
	@echo "Usual sequence for developer"
	@echo "+ initially as root:        make prepare"
	@echo "+ initially as normal user: make menuconfig"
	@echo "+ cyclic as normal user:    make install (this includes build)"
	@echo "*************************************"

all:	build install

build: configuration/include/autoconf.h runtime compiler

runtime:
	(cd runtime; make all; )

compiler:
	(cd compiler/src; make all; )

prepare:
ifeq ($(USERID),0)
	   (cd runtime; make prepare; )
	   (cd imc; make prepare; )
	   (cd compiler; make prepare; )
	   touch $(CONFIG_INSTALL_Target)/bin/prl
	   chmod 777 $(CONFIG_INSTALL_Target)/bin/prl
else
	   $(error You must be root to execute this command)
endif

install: configuration/include/autoconf.h  flashinterface.inc
	(cd runtime; make clean; )
	(cd compiler/src; make clean; )
	(cd imc; make clean; )
	(cd imc; make install; )
	(cd runtime; make install; )
	(cd compiler/src; make install; )
ifeq ($(CONFIG_DOC_PREPORT),y)
	(cd language_report; make clean; )
	(cd language_report; make final; )
endif
	runtime/includeComposer/ic  <prl.inc >prl
	# is created by includeComposer but not needed here
	@rm -f sysincs.h
	@cp prl $(CONFIG_INSTALL_Target)/bin

# create rule to run menuconfig, if no configuration/include/autoconf.h exists
#ifeq ($(wildcard "configuration/include/autoconf.h")),)

configuration/.config:
	(cd configuration; make menuconfig; cd ..)

.PHONY: configuration/include/autoconf.h
configuration/include/autoconf.h:  configuration/.config
ifneq ($(wildcard ./configuration/include/autoconf.*),./configuration/include/autoconf.h)
	(cd configuration; make include/autoconf.h; cd ..)
endif

clean:
	(cd imc; make clean; )
	(cd runtime; make clean; )
	(cd compiler/src; make clean; )
	(cd testsuite; make clean; )
	(cd language_report; make clean; )
	(cd configuration; make clean;)
	rm -f prl

menuconfig:
	(cd configuration; make menuconfig; ./scripts/kconfig/conf -s fm/main.fm; cd .. )

#.PHONY: jenkins_prepare
#jenkins_prepare:
#	cp configuration/jenkins.config configuration/.config
#	(cd configuration; make include/autoconf.h; cd ..)
#
#jenkins: jenkins_prepare
#	make install

flashinterface.inc: ./configuration/include/auto.conf
	@awk -F"=" 							\
	   'BEGIN { found = "CONFIG_INSTALL_FLASHINTERFACE_is_not_set" }\
	    $$1=="CONFIG_INSTALL_FLASHINTERFACE" { found = $$2 }	\
	    END {							\
		    print "INTERFACE=" found	\
                }'  $< >flashinterface.inc





