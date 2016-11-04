# this makefile needs GNU make, since 'shell' is a GNU make extension
# and filter-out
#
# first get list of all PRL-files to check 
# well known problems may be removed from this list to
# perform regression tests
#  remove tests from run: DONOTRUN

# ---
# these programs are known to 'do not work'
DONOTRUN=

# ----
# these programs are known to deliver 'false' if run as normal user
DO_NOT_RUN_IF_NOT_ROOT = 600_two_tasks       611_prio_range_linux \
	617_activatePrio_activate 700_oneSemaphore_samePrio \
	701_twoSemaphore_samePrio 702_semaphore_blocked_after_request \
	704_counting_semaphore    705_Continue_Semaphore           \
	707_two_counting_semaphore      


####################################################
##### do not change beyond this line  ##############

USERID=$(shell id -u)

ifneq ($(USERID),0)
DONOTRUN += $(DO_NOT_RUN_IF_NOT_ROOT)
endif

.SUFFIXES: .prl

.prl:
	prl -b linux $<

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
	@../run-compile.sh 1 $(TARGETS)


allrun:	
	@echo "run test programs"
	@echo "-----------------" 
	@../runtest.sh 1 $(TARGETS)

clean:
	rm -f $(TARGETS)
	rm -f *.cc *.o *.out *.log *.xml
	rm -f system.cc
	rm -f pearl_log.txt testLog.txt

	./clean_m4.sh
	

run: 
	@echo "run simple test programs"
	@echo "------------------------"
	@../runtest.sh 1 $(RUN)
	@echo "excluded (NOT run): " $(DONOTRUN)

doesnotrun: 
	@echo "verify that none of these programs work"
	@echo "------------------------"
	@../runtest.sh 2 $(DONOTRUN)

 
