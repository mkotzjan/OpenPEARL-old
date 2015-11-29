# read system configuration setting from menuconfig
include ../../configuration/.config

include makefile.conf
CC := $(CROSS)$(CC)
CXX := $(CROSS)$(CXX)
AR := $(CROSS)$(AR)

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
BOARDSUPPORT += lpc17_systime.c
BOARDSUPPORT += lpc17_systime_rtcsynced.c
BOARDSUPPORT += lpc17_rtc.c
BOARDSUPPORT += lpc17_systeminit.c


#$(warning BOARDSUPPORT = $(BOARDSUPPORT))

# ------------------------
INCS:=$(addprefix -I ,$(INCS))
PURECXXFLAGS := $(CXXFLAGS)
PURECFLAGS := $(CFLAGS)
CFLAGS += $(INCS)
CXXFLAGS += $(INCS)

# ----------------- all input sources for dependencies
SRCS = $(BOARDSUPPORT)  $(CXXSRCS) $(CSRC)

# ------------------- all files not to be added into lib
BOARDSUPPORT_OBJ := lpc1768_boardsupport.o \
		 $(addsuffix .o,$(basename $(BOARDSUPPORT)))

INCLUDES := $(addsuffix .h,$(basename $(CXXSRCS)))
PLIB_OBJS := $(addsuffix .o,$(basename $(CXXSRCS)))
PLIB_OBJS += $(addsuffix .o,$(basename $(CSRC)))

.PHONY: all clean

targets := OpenPEARLlpc1768_board.o libOpenPEARLlpc1768.a
all: depend $(targets)

startup_lpc1768.o: startup_lpc1768.S
	@echo "[assemble] startup_ARMCM3.S"
	@$(CC) $^ $(CFLAGS) -c
#       @$(CC) $^ $(CFLAGS) -D__NO_SYSTEM_INIT -c

OpenPEARLlpc1768_board.o: $(addsuffix .o,$(basename $(BOARDSUPPORT)))
	@echo $(BOARDSUPPORT)
	@echo $(BOARDSUPPORT)
	@echo $(basename $(BOARDSUPPORT))
	@echo $(addsuffix .o,$(basename $(BOARDSUPPORT)))
	$(CC) -nostdlib -Xlinker -r $(NOHOST) $(CFLAGS) $(INCS) -o $@ $^

.PHONY: libOpenPEARLlpc1768.a
libOpenPEARLlpc1768.a:  $(PLIB_OBJS)
	echo $(PLIB_OBJS)
	$(AR) r libOpenPEARLlpc1768.a $(PLIB_OBJS)

.PHONY: depend
depend:
	@$(CC) -MM $(CFLAGS) $(INCS) $(SRCS) >.depend

.PHONY:clean
clean:
	rm -f $(BOARDSUPPORT_OBJ)
	rm -f $(targets)
	rm -f cc_bin.inc run_bin.inc
	rm -f $(PLIB_OBJS)


tests/task1: tests/task1.cc libopenpearl.a lpc1768_boardsupport.o
	$(CXX) $^ -o $@ $(CFLAGS) $(CXXFLAGS) $(LFLAGS)

.PHONY: install
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


#cpp: tests/cpp.hex
#	lpc21isp -control -term -hex $< /dev/ttyUSB0 115200 12000

#firsttest: tests/firsttest.hex
#	lpc21isp -control -term -hex $< /dev/ttyUSB0 115200 12000

#Post.o: src/Post.cc
#	$(CROSS)$(CXX) $(CXXFLAGS) $(INCS) $(CFLAGS) -c -o $@ $^

-include .depend
