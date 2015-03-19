# this makefile needs GNU make, since 'shell' is a GNU make extension
# and filter-out
#
# first get list of all PRL-files to check 
# well known problems may be removed from this list to
# perform regression tests
#  remove tests from build: DONOTBUILD
#  remove tests from run: DONOTRUN

DONOTBUILD=006_bitstring 030_signals 031_signalsInduce 010_cpp_trycatch \
	201_bit_init \
	400_char_dcl 401_char_expr1 \
	700_dation_rw 701_dation_pg 702_dation_ts 
DONOTRUN=000_empty 006_bitstring \
	703_userdation_dcl 704_dation_open 705_dation_close

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
	@echo "make redo     : perform regression tests: make build; make run  "
	@echo "make allbuild : tests for compile pass1 and pass2 (all targets)"
	@echo "make build    : tests for compile pass1 and pass2 (except known problems)"
	@echo "make run      : tests for primitive run results (except known problems)"
	@echo "make allrun   : tests for primitive run results (all targets)"
	@echo "make clean    : remove all output files"
	@echo "make help     : this output"
	@echo

redo:	build run

allbuild:
	@echo "try to build test programs"
	@echo "--------------------------"
	@./run-compile.sh $(BINS)

build:
	@echo "try to build test programs"
	@echo "--------------------------"
	@./run-compile.sh $(BUILD)
	@echo "excluded (NOT build): " $(DONOTBUILD)

allrun:
	@echo "run simple test programs"
	@echo "------------------------"
	@$(foreach f,$(BINS), ./runtest.sh $(f);)

run: 
	@echo "run simple test programs"
	@echo "------------------------"
	@$(foreach f,$(RUN), ./runtest.sh $(f);)
	@echo "excluded (NOT run): " $(DONOTRUN)

clean:
	rm -f $(BINS)
	rm -f *.cc
	rm -f *.out
	rm -f *.log
	rm -f pearl_log.txt
