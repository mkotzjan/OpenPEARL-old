# this makefile needs GNU make, since 'shell' is a GNU make extension
# and filter-out
#

####################################################
##### do not change beyond this line  ##############

.SUFFIXES: .prl

.prl:
	prl_bin $<

FILES=$(shell ls *.prl)

TARGETS=$(FILES:%.prl=%)


help:
	@echo "make help     : this output"
	@echo "make build    : build all tests"
	@echo "make clean    : remove all output files"
	@echo "make all      : build all tests" 

all:    build 

build: 
	@echo "try to build test programs"
	@echo "--------------------------"
	@./try-compile.sh


clean:
	rm -f $(TARGETS)
	rm -f *.cc
	rm -f *.o
	rm -f *.prl.out
	rm -f *.log
	./clean_m4.sh
