# read system configuration setting from menuconfig
include ../../configuration/.config


include makefile.conf
CC := $(CROSS)$(CC)
CXX := $(CROSS)$(CXX)
AR := $(CROSS)$(AR)

# make silent compilations
COMPILE.c = @echo compile $<; $(CC) $(CFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = @echo compile $<; $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(LINK.cc)
#LINK = @echo link $@; $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@
LINK = @echo link $@; $(CXX) $(PURECFLAGS) -DTARGET=2 $(CXXFLAGS) $(CPPFLAGS) $(LFLAGS) $(LDFLAGS) $^ -o $@



IC_DIR = ../includeComposer
IC = $(IC_DIR)/ic
DWS = $(IC_DIR)/dws

INCS = .
CXXSRCS =
CRCS =
# -------------------- add CMSIS support to project
INCS += ../cortexM/CMSIS/Include

# -------------------- add FreeRTOS support to project
include ../FreeRTOS/FreeRTOS/Files
CSRC += $(addprefix ../FreeRTOS/FreeRTOS/,$(RTOSCSRC))
INCS += $(addprefix ../FreeRTOS/FreeRTOS/,$(RTOSINCS))

# -------------------- add FreeRTOS add on files to project
include ../FreeRTOS/addOns/Files
CSRC += $(addprefix ../FreeRTOS/addOns/,$(ADDONCSRC))
INCS += $(addprefix ../FreeRTOS/addOns/,$(ADDONINCS))
 
# ------------------- add FreeRTOS specific PEARL classes to project
include ../FreeRTOS/PEARL/Files
CXXSRCS += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_SRC))
INCS += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_INC))

# ------------------- add plattform independent PEARL classes to project
include ../common/Files.common
CXXSRCS += $(addprefix ../common/,$(CXX_COMMON))
INCS += ../common

#---------------------------------
# LPCopen V2.xx stuff
# directory, includes directories for LPCopen V2.xx
LPCOpen= ../cortexM/LPCOpen/lpc_chip/chip_17xx_40xx
INCS += $(LPCOpen) $(LPCOpen)/../chip_common $(LPCOpen)/config_175x_6x 

LPCOpen_CSRC += $(LPCOpen)/uart_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/chip_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/clock_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/rtc_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/iocon_17xx_40xx.c

#CSRC += 3rdparty/display/hy32a.c
#CSRC += 3rdparty/display/AsciiLib.c

# ------- add board specific stuff to project
BOARDSUPPORT := startup_lpc1768.S $(LPCOpen_CSRC)
BOARDSUPPORT += lpc17_uart_retarget.c
BOARDSUPPORT += lpc17_oscillators.c
ifeq ($(CONFIG_LPC1768_FreeRTOSHRTimer),y)
  BOARDSUPPORT += lpc17_systime.c
  BOARDSUPPORT += lpc17_systime_rtcsynced.c
endif
BOARDSUPPORT += lpc17_rtc.c
BOARDSUPPORT += lpc17_clockinit.c

#$(warning BOARDSUPPORT = $(BOARDSUPPORT))
# ---------------- add remaining board depending modules


# ------------------------
INCS:=$(addprefix -I ,$(INCS))
PURECXXFLAGS := $(CXXFLAGS)
PURECFLAGS := $(CFLAGS)
CFLAGS += $(INCS)
CXXFLAGS += $(INCS)

# ----------------- all input sources for dependencies
SRCS = $(BOARDSUPPORT)  $(CXXSRCS) $(CSRC)
CHECK_FILES = $(sort  $(SRCS))

# ------------------- all files not to be added into lib
BOARDSUPPORT_OBJ := lpc1768_boardsupport.o \
		 $(addsuffix .o,$(basename $(BOARDSUPPORT)))

INCLUDES := $(addsuffix .h,$(basename $(CXXSRCS)))
PLIB_OBJS := $(addsuffix .o,$(basename $(CXXSRCS)))
PLIB_OBJS += $(addsuffix .o,$(basename $(CSRC)))

phony = all clean

targets := OpenPEARLlpc1768_board.o libOpenPEARLlpc1768.a

_all: all

ifeq ($(CONFIG_LPC1768_tests),y)
  include tests/Makefile.inc
  CHECK_FILES += $(GTEST_SRC) $(GTEST_DATION_SRC) $(TEST_SRC)
  OBJ := $(addsuffix .o,$(basename $(CXX_COMMON_TESTS)))
$(warning OBJ=$(OBJ))

  OBJ := $(addprefix ../common/tests/,$(OBJ))
$(warning OBJ=$(OBJ))
endif

CHECK_FILES := $(sort $(CHECK_FILES))

all: depend $(targets)

startup_lpc1768.o: startup_lpc1768.S
	@echo "[assemble] startup_ARMCM3.S"
	@$(CC) $^ $(CFLAGS) -c
#       @$(CC) $^ $(CFLAGS) -D__NO_SYSTEM_INIT -c

OpenPEARLlpc1768_board.o: $(addsuffix .o,$(basename $(BOARDSUPPORT)))
	@echo "[create] $@"
	@$(CC) -nostdlib -Xlinker -r $(NOHOST) $(CFLAGS) $(INCS) -o $@ $^

phony += libOpenPEARLlpc1768.a
libOpenPEARLlpc1768.a:  $(PLIB_OBJS)
	@echo "[create] $@"
	@$(AR) r libOpenPEARLlpc1768.a $(PLIB_OBJS)

phony += depend
depend: ../common/Signals.hh ../common/Signals.hcc $(SRCS)
	@echo "[create] .depend"
	$(CXX) -MM $(CFLAGS) $(INCS) $^ >.depend

phony += clean
clean:
	@rm -f $(BOARDSUPPORT_OBJ)
	@rm -f $(targets)
	@rm -f cc_bin.inc run_bin.inc
	@rm -f .depend pearl.h
	@rm -f $(PLIB_OBJS) $(OBJ)


tests/task1: tests/task1.cc libopenpearl.a lpc1768_boardsupport.o
	$(CXX) $^ -o $@ $(CFLAGS) $(CXXFLAGS) $(LFLAGS)

phony += install
install: all
	rm -f pearl2.h
	$(IC) -c $(INCS)  <PearlIncludes.h | $(DWS) >pearl2.h
	cat sysincs.h pearl2.h >pearl.h
	rm -f pearl2.h sysincs.h
	cp libOpenPEARLlpc1768.a $(CONFIG_INSTALL_Target)/lib
	mkdir -p $(CONFIG_INSTALL_Target)/lib/ldscripts
	cp OpenPEARLlpc1768.ld $(CONFIG_INSTALL_Target)/lib/ldscripts
	cp OpenPEARLlpc1768_board.o $(CONFIG_INSTALL_Target)/lib
	#
	# create cc_bin.inc
	echo '$(CXX) -DTARGET=2 $(PURECFLAGS) $(CXXFLAGS) $(LFLAGS) $(CONFIG_INSTALL_Target)/lib/OpenPEARLlpc1768_board.o -I $(CONFIG_INSTALL_Target)/include $(CONFIG_INSTALL_Target)/lib/libOpenPEARLlpc1768.a $$fn.cc -o $$fn ' > cc_bin.inc 
	#
	# create run_bin.inc
	echo "# auto generated " > run_bin.inc
	echo 'objcopy -O ihex $$fn $$fn.hex' >>run_bin.inc
	echo 'lpc21isp -control -term -hex $$fn.hex $$INTERFACE 115200 12000' >>run_bin.inc

# -------------------------------------------------
# additional rules to enable make to build the autogenerated files
#
../common/Signals.hh: ../common/Signals.ods
	@echo create signal definitions from spreadsheet
	@(cd ../common; \
	 perl GenerateSignalDefinitions.pl GENERIC 2>signals.log;)

../common/Signals.hcc: ../common/Signals.ods
	(cd ../common; perl GenerateSignalDefinitions.pl GENERIC; )

../Signals.cc: ../Signals.hh ../Signals.hcc

phony +=  checkformat
checkformat:
	@$(foreach f,$(CHECK_FILES),../codestyle/check.sh $(f);)


#cpp: tests/cpp.hex
#	lpc21isp -control -term -hex $< /dev/ttyUSB0 115200 12000

#firsttest: tests/firsttest.hex
#	lpc21isp -control -term -hex $< /dev/ttyUSB0 115200 12000

#Post.o: src/Post.cc
#	$(CROSS)$(CXX) $(CXXFLAGS) $(INCS) $(CFLAGS) -c -o $@ $^

-include .depend

phony += info
info:
	@echo "Makefile Summary"
	@echo "targets: " $(targets)
	@echo "CFLAGS " $(CFLAGS)
	@echo "INCS " $(INCS)
	@echo "CXXFLAGS " $(CXXFLAGS)
	@echo "LFLAGS " $(LFLAGS)
	@echo "LDFLAGS " $(LDFLAGS)
	@echo "PHONY " $(phony)

.PHONY: $(phony)
