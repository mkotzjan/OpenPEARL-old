# read system configuration setting from menuconfig
include ../../configuration/.config

phony = all clean
_all: all


include makefile.conf

CC := $(CROSS)$(CC)
CXX := $(CROSS)$(CXX)
AR := $(CROSS)$(AR)

# make silent compilations
COMPILE.c = @echo compile $<; $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = @echo [compile] $<; $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
#COMPILE.cc = echo [compile] $<; $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(LINK.cc)
#LINK = @echo [link] $@; $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ -o $@
LINK = @echo [link] $@; $(CXX) $(PURECFLAGS) -DTARGET=2 $(CXXFLAGS) $(CPPFLAGS) $(LFLAGS) $(LDFLAGS) $^ -o $@

IC_DIR = ../includeComposer
IC = $(IC_DIR)/ic
DWS = $(IC_DIR)/dws

INCS = .
CXXSRCS =
CRCS =

#CHAN_FS= ../FreeRTOS/chanFatFs/src
#LPCOPEN_FS= ../cortexM/LPCOpen/software/filesystems

targets := libOpenPEARLesp32.a esp32.xml

# -------------------- add CMSIS support to project
#INCS += ../cortexM/CMSIS/Include

# -------------------- add FreeRTOS support to project
#include ../FreeRTOS/FreeRTOS/Files
#CSRC += $(addprefix ../FreeRTOS/FreeRTOS/,$(RTOSCSRC))
#INCS += $(addprefix ../FreeRTOS/FreeRTOS/,$(RTOSINCS))

# -------------------- add FreeRTOS addOn files to project
#include ../FreeRTOS/addOns/Files
#CSRC += $(addprefix ../FreeRTOS/addOns/,$(ADDONCSRC))
#CHECK_FILES += $(addprefix ../FreeRTOS/addOns/,$(ADDONCSRC))
#INCS += $(addprefix ../FreeRTOS/addOns/,$(ADDONINCS))

# ------------------- add FreeRTOS specific PEARL classes to project
#include ../FreeRTOS/PEARL/Files
#CXXSRCS += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_SRC))
#CHECK_FILES += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_SRC))
#CHECK_FILES += $(addprefix ../FreeRTOS/PEARL/,$(addsuffix .h,$(basename $(PEARL_FREERTOS_SRC))))
#INCS += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_INC))
#XMLFILES += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_XML))

# -------------------- add cortex-M clock to project
#CXXSRCS += ../FreeRTOS/addOns/CortexMClock.cc
#CHECK_FILES += ../FreeRTOS/addOns/CortexMClock.cc

# ------------------- add plattform independent PEARL classes to project
include ../common/Files.common
CXXSRCS += $(addprefix ../common/,$(CXX_COMMON))
CHECK_FILES += $(addprefix ../common/,$(CXX_COMMON))
INCS += ../common
XMLFILES  += $(addprefix ../common/,$(XML_COMMON))

#---------------------------------
# required parts of LPCopen V2.xx 
# directory, includes directories for LPCopen V2.xx
#LPCOpen= ../cortexM/LPCOpen/lpc_chip/chip_17xx_40xx
#INCS += $(LPCOpen) $(LPCOpen)/../chip_common $(LPCOpen)/config_175x_6x 

#LPCOpen_CSRC += $(LPCOpen)/uart_17xx_40xx.c
#LPCOpen_CSRC += $(LPCOpen)/chip_17xx_40xx.c
#LPCOpen_CSRC += $(LPCOpen)/clock_17xx_40xx.c
#LPCOpen_CSRC += $(LPCOpen)/rtc_17xx_40xx.c
#LPCOpen_CSRC += $(LPCOpen)/iocon_17xx_40xx.c

#CXXSRCS += main.cc Log.cc
#CHECK_FILES += main.cc Log.cc

#$(warning BOARDSUPPORT = $(BOARDSUPPORT))

# ---------------- add remaining board depending modules
# ----------------- add test applications
#TESTOBJ:=
#ifeq ($(CONFIG_LPC1768_tests),y)
#  include tests/Makefile.inc
#  CHECK_FILES += $(GTEST_SRC) $(GTEST_DATION_SRC) $(TEST_SRC)
#  OBJ := $(addsuffix .o,$(basename $(CXX_COMMON_TESTS)))
#  OBJ := $(addprefix ../common/tests/,$(OBJ))
#  #$(warning OBJ=$(OBJ))
##  $(warning targets=$(targets))
#endif

# ------------------------ finalize variables 
#INCS:=$(addprefix -I ,$(INCS))
#PURECXXFLAGS := $(CXXFLAGS)
#ifeq ($(CONFIG_LPC1768_CHECK_STACK_OVERFLOW),y)
#PURECXXFLAGS += -finstrument-functions 
#endif

PURECFLAGS := $(CFLAGS)
CFLAGS += $(INCS)
CXXFLAGS += $(INCS)
CXXFLAGS += -DTARGET=2

# ----------------- all input sources for dependencies
SRCS = $(BOARDSUPPORT)  $(CXXSRCS) $(CSRC) $(LPCOpen_CSRC)

# ------------------- all files not to be added into lib
BOARDSUPPORT_OBJ := \
		 $(addsuffix .o,$(basename $(BOARDSUPPORT))) \
		 $(addsuffix .o,$(basename $(LPCOpen_CSRC)))

INCLUDES := $(addsuffix .h,$(basename $(CXXSRCS)))
#PLIB_OBJS +=  $(addsuffix .o,$(basename $(LPCOpen_CSRC)))
PLIB_OBJS := $(addsuffix .o,$(basename $(CXXSRCS)))
PLIB_OBJS += $(addsuffix .o,$(basename $(CSRC)))


CHECK_FILES := $(sort $(CHECK_FILES))

#$(warning BOARDSUPPORT_OBJ=$(BOARDSUPPORT_OBJ))
#$(warning XMLFILES=$(XMLFILES))

all: depend $(targets)

tests/testStackOverflow.s: tests/testStackOverflow.cc
	$(CXX) $^ -S $(CXXFLAGS)  $(PURECXXFLAGS)

libOpenPEARLesp32.a:  $(PLIB_OBJS)
	@echo "[create] $@"
	@rm   -f libOpenPEARLesp32.a
	@$(AR) r libOpenPEARLesp32.a $(PLIB_OBJS)

esp32.xml: $(XMLFILES)
	@echo "[create] $@"
	#echo $(XMLFILES)
	@echo "[build] $@"
	@echo "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" > $@
	@echo "<platform file=\"$@\">" >> $@
	@cat $(XMLFILES) >> $@
	@echo "</platform>" >> $@


phony += depend
depend: ../common/Signals.hh ../common/Signals.hcc $(SRCS)
	@echo "[create] $@"
	@$(CXX) -MM $(CFLAGS) $(CPPFLAGS) $(INCS) $^ >.depend

phony += clean
clean:
	@rm -f $(BOARDSUPPORT_OBJ)
	@rm -f $(targets)
	@rm -f $(GTEST_OBJS)
	@rm -f $(GTEST_OBJS1)
	@rm -f $(GTEST_OBJS2)
	@rm -f cc_bin.inc run_bin.inc
	@rm -f .depend pearl.h
	@rm -f $(PLIB_OBJS) $(OBJ)
	@rm -f $(addsuffix .diff,$(CHECK_FILES))
	@rm -f $(addsuffix .new,$(CHECK_FILES))
	@rm -rf html
	@rm -rf latex
	@rm -f ../common/tests/*.o
	@rm -f tests/*.o

phony += doc
doc:
	doxygen doc_stuff/Doxyfile
#	(cd latex; make pdf; cd ..)

phony += prepare
prepare:
	@mkdir -p $(CONFIG_INSTALL_Target)/lib/ldscripts
	@touch $(CONFIG_INSTALL_Target)/lib/libOpenPEARLesp32.a
	@chmod 666 $(CONFIG_INSTALL_Target)/lib/libOpenPEARLesp32.a
	@touch $(CONFIG_INSTALL_Target)/lib/ldscripts/OpenPEARLesp32.ld
	@chmod 666 $(CONFIG_INSTALL_Target)/lib/ldscripts/OpenPEARLesp32.ld
	@touch $(CONFIG_INSTALL_Target)/lib/esp32.xml
	@chmod 666 $(CONFIG_INSTALL_Target)/lib/esp32.xml

phony += install
install: all cc_bin run_bin
	@echo [install]
	@rm -f pearl2.h
	@$(IC) -c $(INCS)  <PearlIncludes.h | $(DWS) >pearl2.h
	@cat sysincs.h pearl2.h >pearl.h
	@rm -f pearl2.h sysincs.h
	@cp libOpenPEARLesp32.a $(CONFIG_INSTALL_Target)/lib
	@mkdir -p $(CONFIG_INSTALL_Target)/lib/ldscripts
	@cp OpenPEARLesp32.ld $(CONFIG_INSTALL_Target)/lib/ldscripts
	@cp esp32.xml $(CONFIG_INSTALL_Target)/lib

####### TODO
phony += cc_bin
cc_bin:
	@echo "[create] $@"
	@echo '$(CXX) -DTARGET=2 $(PURECXXFLAGS) $(LFLAGS) $(CONFIG_INSTALL_Target)/lib/OpenPEARLlpc1768_board.o -I $(CONFIG_INSTALL_Target)/include $$SYSTEM $$fn.cc -o $$fn $(CONFIG_INSTALL_Target)/lib/libOpenPEARLlpc1768.a' > cc_bin.inc 

######## TODO
phony += run_bin
run_bin:
	@echo "[create] $@"
	@echo "# auto generated " > run_bin.inc
	@echo 'objcopy -O ihex $$fn $$fn.hex' >>run_bin.inc
	@echo 'lpc21isp -control -term -hex $$fn.hex $$INTERFACE 115200 12000' >>run_bin.inc

# -------------------------------------------------
# additional rules to enable make to build the autogenerated files
#
../common/Signals.hh: ../common/Signals.ods
	@echo create signal definitions from spreadsheet
	@(cd ../common; \
	 perl GenerateSignalDefinitions.pl GENERIC ESP32 2>signals.log;)

../common/Signals.hcc: ../common/Signals.ods
	(cd ../common; perl GenerateSignalDefinitions.pl GENERIC ESP32; )

../Signals.cc: ../Signals.hh ../Signals.hcc

phony +=  checkformat
checkformat:
	@$(foreach f,$(CHECK_FILES),../codestyle/check.sh $(f);)

-include .depend

phony += info
info:
	@echo "Makefile Summary"
	@echo "targets: " $(targets)
	@echo "CFLAGS " $(CFLAGS)
	@echo "CPPFLAGS " $(CPPFLAGS)
	@echo "INCS " $(INCS)
	@echo "CXXFLAGS " $(CXXFLAGS)
	@echo "LFLAGS " $(LFLAGS)
	@echo "LDFLAGS " $(LDFLAGS)
	@echo "PHONY " $(phony)

.PHONY: $(phony)
