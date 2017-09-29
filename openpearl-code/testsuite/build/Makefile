# this makefile needs GNU make, since 'shell' is a GNU make extension
# and filter-out
#
# first get list of all PRL-files to check 
# well known problems may be removed from this list to
# perform regression tests
#  remove tests from build: DONOTBUILD
#  remove tests from run: DONOTRUN
#
# --------------------------
# for the usage in jenkins the policy is:
# add all test cases to DONOTBUILD which are recorded in an open ticket
# add all test cases to DONOTRUN which are known that they are intended
#     as build only test cases
# the result of make build is ok, if all test cases work
# the result of make doesnotbuild is ok, if no test cases work
 
DONOTBUILD= 030_signals 031_signalsInduce  \
	043_switch \
	103_fixed_monadic_arith \
	401_char_expr1 \
	701_dation_pg \
	706_convert \
	850_ref_decl

DONOTRUN=000_empty \
	703_userdation_dcl \
	704_dation_open \
	705_dation_close

####################################################
##### do not change beyond this line  ##############
.SUFFIXES:

TESTS=$(shell ls *.prl)
BINS=$(TESTS:%.prl=%)
BUILD=$(filter-out $(DONOTBUILD),$(BINS)) 
RUN=$(filter-out $(DONOTRUN) $(DONOTBUILD) ,$(BINS)) 

.PHONY: help all run build clean allbuild allrun redo
all:	allbuild allrun

help:
	@echo "make          : --> make all    " 
	@echo "make all      : make allbuild; make allrun  "
	@echo "make redo     : perform regression tests: make build,doesnotbuild,run,doesnotrun  "
	@echo "make allbuild : tests for compile pass1 and pass2 (all targets)"
	@echo "make build    : tests for compile pass1 and pass2 (except known problems)"
	@echo "make doesnotbuild    : tests non of the known problems does build"
	@echo "make run      : tests for primitive run results (except known problems)"
	@echo "make doesnotrun  : test that non on known problems work"
	@echo "make allrun   : tests for primitive run results (all targets)"
	@echo "make clean    : remove all output files"
	@echo "make help     : this output"
	@echo

redo:	build doesnotbuild run doesnotrun

allbuild:
	@echo "try to build test programs"
	@echo "--------------------------"
	@../run-compile.sh 0 $(BINS)

build:
	@echo "try to build test programs"
	@echo "--------------------------"
	@../run-compile.sh 1 $(BUILD)
	@echo "excluded (NOT build): " $(DONOTBUILD)

doesnotbuild:
	@echo "none of the programs should build"
	@echo "--------------------------"
	@../run-compile.sh 2 $(DONOTBUILD)

allrun:
	@echo "run simple test programs"
	@echo "------------------------"
	@../runtest.sh 0 $(BINS)

run: 
	@echo "run simple test programs"
	@echo "------------------------"
	@../runtest.sh 1 $(RUN)
	@echo "excluded (NOT run): " $(DONOTRUN)

doesnotrun: 
	@echo "cross check: none of the test cases should run"
	@echo "------------------------"
	@../runtest.sh 2 $(DONOTRUN)

clean:
	rm -f $(BINS)
	rm -f *.cc *.out *.xml *.log *.o
	rm -f pearl_log.txt 
	rm -f system.cc 
	rm -f .log
