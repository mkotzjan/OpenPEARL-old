include makefile.conf
include Files

CORE=CM3

CINCS += ../../../FreeRTOS/include

INCLUDES = $(CINCS) $(CPPINCS)
INCS=$(addprefix -I ,$(INCLUDES))

LDIRS = -L ../../../FreeRTOS
CXXFLAGS += $(CFLAGS) $(INCS) $(LDIRS)
LFLAGS += -lopenpearl -T ../gcc_lpc1768.ld --specs=nosys.specs
OBJS = ../lpc1768_boardsupport.o
# ../lpc1768_boardsupport.o -T ../gcc_lpc1768.ld

.PHONY: all info clean

## This has to do with the testing infrastructure...
#GTEST_SRCS += simpleTests.cc
#CXXFLAGS += -DSIMPLE_TESTS -I .
#GTEST_OBJS=$(GTEST_SRCS:.cc=.o)

all: info $(TARGETS) cpp.elf firsttest.elf

info:
	@echo "CXXFLAGS="
	@echo $(CXXFLAGS)
	@echo
	@echo "LFLAGS="
	@echo $(LFLAGS)

clean:
	rm -f *.elf *.map
	rm -f ../../common/tests/*.o
	rm -f  $(TARGETS)
	rm -f  $(TARGETS:%.hex=%.elf)
	rm -f $(GTEST_OBJS)

%.hex: %.elf

%.elf: %.cc
	@echo "[building and linking]" $@
	$(CROSS)$(CXX) $(CXXFLAGS) $(USE_NOHOST) $(MAP) $^ $(OBJS) $(LFLAGS) -o $@

