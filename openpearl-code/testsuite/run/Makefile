# this makefile needs GNU make, since 'shell' is a GNU make extension
# and filter-out
#
# first get list of all PRL-files to check 
# well known problems may be removed from this list to
# perform regression tests
#  remove tests from run: DONOTRUN

DONOTRUN=#609_suspend_terminate 610_continue_terminate

####################################################
##### do not change beyond this line  ##############

.SUFFIXES: .prl

.prl:
	prl_bin $<

FILES=$(shell ls *.prl)

TARGETS=$(FILES:%.prl=%)

RUN=$(filter-out $(DONOTRUN) ,$(TARGETS)) 


help:
	@echo "make help     : this output"
	@echo "make build    : build all tests"
	@echo "make allrun   : tests for run results (all targets)"
	@echo "make clean    : remove all output files"
	@echo "make run      : tests for run results (except known problems)"
	@echo "make all      : build and tests all files"

all:    build allrun

build: 
	@echo "try to build test programs"
	@echo "--------------------------"
	@./run-compile.sh


allrun:	
	@echo "run test programs"
	@echo "-----------------" 
	@$(foreach f,$(TARGETS), ./startScript.sh $(f);)

clean:
	rm -f $(TARGETS)
	rm -f *.cc
	rm -f *.o
	rm -f *.prl.out
	rm -f *.log

run: 
	@echo "run simple test programs"
	@echo "------------------------"
	@$(foreach f,$(RUN), ./startScript.sh $(f);)
	@echo "excluded (NOT run): " $(DONOTRUN)

 
