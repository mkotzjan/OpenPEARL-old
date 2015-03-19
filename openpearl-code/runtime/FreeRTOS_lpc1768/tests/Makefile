include ../makefile.conf
INCS=$(addprefix -I ../,$(INCLUDES4SUB))

CXXFLAGS=$(CXXFLAGS4SUB) $(INCS) 
LFLAGS=$(LFLAGS4SUB)

.PHONY: all info clean

TARGETS= \
	cpp.hex \
	firsttest.hex \
        interruptTest.hex \
        sched_demo.hex \
        susp_cont_test.hex \
        sematest.hex sematerminate.hex  \

#	DataTypeTests.hex DataTypeTests.elf 
#.elf.hex:


.cc.o:
	@echo "[compile] " $^
	@$(CXX) $(CXXFLAGS) -c $^ -o $@
	
GTEST_SRCS += simpleTests.cc
include ../../common/tests/Files

CXXFLAGS += -DSIMPLE_TESTS -I .

GTEST_OBJS=$(GTEST_SRCS:.cc=.o)

all: info $(TARGETS)

info:
	@echo "CXXFLAGS="
	@echo $(CXXFLAGS)
	@echo
	@echo "LFLAGS="
	@echo $(LFLAGS)
	@echo
	@echo "INCLUDES4SUB="
	@echo $(INCLUDES4SUB)


clean:
	rm -f *.o *.map
	rm -f ../../common/tests/*.o
	rm -f  $(TARGETS)
	rm -f  $(TARGETS:%.hex=%.elf)
	rm -f $(GTEST_OBJS)

firsttest.o: firsttest.cc
	@echo "[compile] " $^
	$(CXX) $(CXXFLAGS) -c $^

firsttest.elf: firsttest.o ../plib.a
	@echo "[linking] " $@
	@$(CXX) $(CXXFLAGS)  $(LFLAGS) $(MAP) $^  -o $@
	@$(SIZE) $@

firsttest.hex: firsttest.elf

interruptTest.elf: ../../common/tests/interruptTest.o ../plib.a
	$(CXX) $(CXXFLAGS) $^ $(LFLAGS) -o $@
	@$(SIZE) $@

sched_demo.elf: ../../common/tests/sched_demo.o ../../common/tests/round_robin_test.o ../plib.a
	$(CXX) $(CXXFLAGS) $^ $(LFLAGS) -o $@ 
	@$(SIZE) $@

susp_cont_test.elf: ../../common/tests/susp_cont_test.o ../plib.a
	$(CXX) $(CXXFLAGS) $^  -o $@  $(LFLAGS) 
	@$(SIZE) $@

sematest.elf: ../../common/tests/sematest.o ../plib.a
	$(CXX) $(CXXFLAGS) $^  -o $@ $(LFLAGS) 
	@$(SIZE) $@

sematerminate.elf: ../../common/tests/sematerminate.o ../plib.a
	$(CXX) $(CXXFLAGS) $^  -o $@ $(LFLAGS)
	@$(SIZE) $@

cpp.o: cpp.cc
	@echo "[compile] " $^
	$(CXX) $(CXXFLAGS) -c $^

#cpp.elf: cpp.o $(PLIB) 
cpp.elf: cpp.o ../plib.a
	@echo "[linking] " $@
	@$(CXX) $(CXXFLAGS)  $(LFLAGS) $(MAP) $^  -o $@
	@$(SIZE) $@

cpp.hex: cpp.elf

simpleTests.o: simpleTests.cc
	$(CXX) $(CXXFLAGS) -c $^


DataTypeTests.elf: simpleTests.o $(GTEST_OBJS) ../plib1.a
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(MAP) $^ -o $@
	@$(SIZE) $@

DataTypeTests.hex: DataTypeTests.elf

