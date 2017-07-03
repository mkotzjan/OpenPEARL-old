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

CHAN_FS= ../FreeRTOS/chanFatFs/src
LPCOPEN_FS= ../cortexM/LPCOpen/software/filesystems

targets := OpenPEARLlpc1768_board.o libOpenPEARLlpc1768.a lpc1768.xml

# -------------------- add CMSIS support to project
INCS += ../cortexM/CMSIS/Include

# -------------------- add FreeRTOS support to project
include ../FreeRTOS/FreeRTOS/Files
CSRC += $(addprefix ../FreeRTOS/FreeRTOS/,$(RTOSCSRC))
INCS += $(addprefix ../FreeRTOS/FreeRTOS/,$(RTOSINCS))

# -------------------- add FreeRTOS addOn files to project
include ../FreeRTOS/addOns/Files
CSRC += $(addprefix ../FreeRTOS/addOns/,$(ADDONCSRC))
CHECK_FILES += $(addprefix ../FreeRTOS/addOns/,$(ADDONCSRC))
INCS += $(addprefix ../FreeRTOS/addOns/,$(ADDONINCS))

# ------------------- add FreeRTOS specific PEARL classes to project
include ../FreeRTOS/PEARL/Files
CXXSRCS += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_SRC))
CHECK_FILES += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_SRC))
CHECK_FILES += $(addprefix ../FreeRTOS/PEARL/,$(addsuffix .h,$(basename $(PEARL_FREERTOS_SRC))))
INCS += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_INC))
XMLFILES += $(addprefix ../FreeRTOS/PEARL/,$(PEARL_FREERTOS_XML))

# -------------------- add cortex-M clock to project
CXXSRCS += ../FreeRTOS/addOns/CortexMClock.cc
CHECK_FILES += ../FreeRTOS/addOns/CortexMClock.cc

# ------------------- add plattform independent PEARL classes to project
include ../common/Files.common
CXXSRCS += $(addprefix ../common/,$(CXX_COMMON))
CHECK_FILES += $(addprefix ../common/,$(CXX_COMMON))
INCS += ../common
XMLFILES  += $(addprefix ../common/,$(XML_COMMON))

#---------------------------------
# required parts of LPCopen V2.xx 
# directory, includes directories for LPCopen V2.xx
LPCOpen= ../cortexM/LPCOpen/lpc_chip/chip_17xx_40xx
INCS += $(LPCOpen) $(LPCOpen)/../chip_common $(LPCOpen)/config_175x_6x 

LPCOpen_CSRC += $(LPCOpen)/uart_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/chip_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/clock_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/rtc_17xx_40xx.c
LPCOpen_CSRC += $(LPCOpen)/iocon_17xx_40xx.c

CXXSRCS += main.cc Log.cc
CHECK_FILES += main.cc Log.cc

ifeq ($(CONFIG_LPC1768_HY32A),y)
CSRC += Hy32.cc Hy32LowLevel.cc SmallFont.c BigFont.c
XMLFILES += Hy32.xml
CHECK_FILES +=  Hy32.cc Hy32LowLevel.cc \
	Hy32.h Hy32LowLevel.h
endif

# ------- add board specific stuff to project
BOARDSUPPORT := 
#BOARDSUPPORT += lpc17_uart_retarget.c
BOARDSUPPORT += lpc17_oscillators.c
BOARDSUPPORT += lpc17_SystemInit.c
BOARDSUPPORT += Lpc17xxRTC.cc
BOARDSUPPORT += Post.cc
XMLFILES     += Post.xml
BOARDSUPPORT += SystemConsole.cc
BOARDSUPPORT += Lpc17xxTimer0.cc
BOARDSUPPORT += Lpc17xxClock.cc
XMLFILES     += Lpc17xxClock.xml
BOARDSUPPORT += Lpc17xxDigitalIn.cc
BOARDSUPPORT += Lpc17xxDigitalOut.cc
XMLFILES     += Lpc17xxDigitalIn.xml Lpc17xxDigitalOut.xml
BOARDSUPPORT += Lpc17xxInterrupt.cc
XMLFILES     += Lpc17xxInterrupt.xml
BOARDSUPPORT += Lpc17xxUartInternal.cc
BOARDSUPPORT += Lpc17xxUart.cc
XMLFILES     += Lpc17xxUart.xml
CHECK_FILES += $(BOARDSUPPORT)
CHECK_FILES += $(addsuffix .h,$(basename $(BOARDSUPPORT)))

### do not check the assembly source
BOARDSUPPORT += startup_lpc1768.S 

BOARDSUPPORT += Lpc17xxISRProxy.cc
CHECK_FILES += Lpc17xxISRProxy.cc  Lpc17xxISRProxy.h

ifeq ($(CONFIG_LPC1768_CHECK_STACK_OVERFLOW),y)
BOARDSUPPORT += ../FreeRTOS/addOns/testStackOverflow.cc
endif
ifeq ($(CONFIG_LPC1768_CAN),y)
LPCOpen_CSRC += $(LPCOpen)/can_17xx_40xx.c
BOARDSUPPORT += Lpc17xxCan.cc
XMLFILES     += Lpc17xxCan.xml
endif
ifeq ($(CONFIG_LPC1768_HAS_USB),y)
CPPFLAGS += -D__LPC175X_6X__ -DUSB_HOST_ONLY
LPCUSB_Base= ../cortexM/LPCOpen/software/LPCUSBLib/Drivers/USB
LPCUSB_Core= $(LPCUSB_Base)/Core
LPCUSB_Class= $(LPCUSB_Base)/Class
LPCOpen_CSRC += $(LPCUSB_Core)/Host.c
LPCOpen_CSRC += $(LPCUSB_Core)/HostStandardReq.c
LPCOpen_CSRC += $(LPCUSB_Core)/USBTask.c
LPCOpen_CSRC += $(LPCUSB_Core)/ConfigDescriptor.c
LPCOpen_CSRC += $(LPCUSB_Core)/Pipe.c
LPCOpen_CSRC += $(LPCUSB_Core)/PipeStream.c
LPCOpen_CSRC += $(LPCUSB_Core)/USBMemory.c
LPCOpen_CSRC += $(LPCUSB_Core)/USBController.c
LPCOpen_CSRC += $(LPCUSB_Core)/HCD/HCD.c
LPCOpen_CSRC += $(LPCUSB_Core)/HCD/OHCI/OHCI.c
LPCOpen_CSRC += $(LPCUSB_Core)/HAL/LPC17XX/HAL_LPC17xx.c
CXXSRCS += Lpc17xxUsb.cc
CHECK_FILES += Lpc17xxUsb.cc  Lpc17xxUsb.h
INCS += $(LPCUSB_Base) 
endif

ifeq ($(CONFIG_HAS_FAT),y)
   INCS += $(CHAN_FS)
   LPCOpen_CSRC += $(CHAN_FS)/ff.c
   LPCOpen_CSRC += $(CHAN_FS)/ff_errors.c
   LPCOpen_CSRC += $(CHAN_FS)/diskio.c
endif

ifeq ($(CONFIG_LPC1768_USB_Disk),y)
   LPCOpen_CSRC += $(LPCUSB_Class)/Host/MassStorageClassHost.c
   CXXSRCS += Lpc17xxUsbDisk.cc
   CHECK_FILES += Lpc17xxUsbDisk.cc Lpc17xxUsbDisk.h
   XMLFILES += Lpc17xxUsbDisk.xml
   INCS += $(LPCUSB_Base)/Class/Host 
endif

ifeq ($(CONFIG_LPC1768_USB_Keyboard),y)
   LPCOpen_CSRC += $(LPCUSB_Class)/Host/HIDClassHost.c
   LPCOpen_CSRC += $(LPCUSB_Class)/Common/HIDParser.c
   CXXSRCS += Lpc17xxUsbKeyboard.cc
#CHECK_FILES += Lpc17xxUsbKeyboard.cc Lpc17xxUsbKeyboard.h
   CHECK_FILES += Lpc17xxUsbKeyboard.h
   XMLFILES += Lpc17xxUsbKeyboard.xml
endif

ifeq ($(CONFIG_LPC1768_SDCARD),y)
   LPCOpen_CSRC +=  $(LPCOpen)/spi_17xx_40xx.c
   LPCOpen_CSRC +=  $(LPCOpen)/ssp_17xx_40xx.c
#   LPCOpen_CSRC +=  $(LPCOpen)/gpdma_17xx_40xx.c
   CXXSRCS += Lpc17xxSDCard.cc
   CHECK_FILES += Lpc17xxSDCard.cc Lpc17xxSDCard.h
   XMLFILES += Lpc17xxSDCard.xml
endif

#$(warning BOARDSUPPORT = $(BOARDSUPPORT))

# ---------------- add remaining board depending modules
# ----------------- add test applications
TESTOBJ:=
ifeq ($(CONFIG_LPC1768_tests),y)
  include tests/Makefile.inc
  CHECK_FILES += $(GTEST_SRC) $(GTEST_DATION_SRC) $(TEST_SRC)
  OBJ := $(addsuffix .o,$(basename $(CXX_COMMON_TESTS)))
  OBJ := $(addprefix ../common/tests/,$(OBJ))
  #$(warning OBJ=$(OBJ))
#  $(warning targets=$(targets))
endif

# ------------------------ finalize variables 
INCS:=$(addprefix -I ,$(INCS))
PURECXXFLAGS := $(CXXFLAGS)
ifeq ($(CONFIG_LPC1768_CHECK_STACK_OVERFLOW),y)
PURECXXFLAGS += -finstrument-functions 
endif

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

startup_lpc1768.o: startup_lpc1768.S
	@echo "[assemble] startup_ARMCM3.S"
	@$(CC) $^ $(CFLAGS) -c

OpenPEARLlpc1768_board.o: $(BOARDSUPPORT_OBJ)
	@echo "[create] $@"
	@$(CC) -nostdlib -Xlinker -r $(NOHOST) $(CFLAGS) $(INCS) -o $@ $^

libOpenPEARLlpc1768.a:  $(PLIB_OBJS)
	@echo "[create] $@"
	@rm   -f libOpenPEARLlpc1768.a
	@$(AR) r libOpenPEARLlpc1768.a $(PLIB_OBJS)

lpc1768.xml: $(XMLFILES)
	echo $(XMLFILES)
	@echo "[build] $@"
	@echo "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" > $@
	@echo "<platform file=\"$@\">" >> $@
	@cat $(XMLFILES) >> $@
	@echo "</platform>" >> $@


phony += depend
depend: ../common/Signals.hh ../common/Signals.hcc $(SRCS)
	@echo "[create] .depend"
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

phony += doc
doc:
	doxygen doc_stuff/Doxyfile
#	(cd latex; make pdf; cd ..)

phony += prepare
prepare:
	@touch $(CONFIG_INSTALL_Target)/lib/libOpenPEARLlpc1768.a
	@chmod 666 $(CONFIG_INSTALL_Target)/lib/libOpenPEARLlpc1768.a
	@touch $(CONFIG_INSTALL_Target)/lib/OpenPEARLlpc1768_board.o
	@chmod 666 $(CONFIG_INSTALL_Target)/lib/OpenPEARLlpc1768_board.o
	@touch $(CONFIG_INSTALL_Target)/lib/ldscripts/OpenPEARLlpc1768.ld
	@mkdir -p $(CONFIG_INSTALL_Target)/lib/ldscripts
	@chmod 666 $(CONFIG_INSTALL_Target)/lib/ldscripts/OpenPEARLlpc1768.ld
	@touch $(CONFIG_INSTALL_Target)/lib/lpc1768.xml
	@chmod 666 $(CONFIG_INSTALL_Target)/lib/lpc1768.xml

phony += install
install: all cc_bin run_bin
	rm -f pearl2.h
	$(IC) -c $(INCS)  <PearlIncludes.h | $(DWS) >pearl2.h
	cat sysincs.h pearl2.h >pearl.h
	rm -f pearl2.h sysincs.h
	cp libOpenPEARLlpc1768.a $(CONFIG_INSTALL_Target)/lib
	mkdir -p $(CONFIG_INSTALL_Target)/lib/ldscripts
	cp OpenPEARLlpc1768.ld $(CONFIG_INSTALL_Target)/lib/ldscripts
	cp OpenPEARLlpc1768_board.o $(CONFIG_INSTALL_Target)/lib
	cp lpc1768.xml $(CONFIG_INSTALL_Target)/lib

phony += cc_bin
cc_bin:
	# create cc_bin.inc
	echo '$(CXX) -DTARGET=2 $(PURECXXFLAGS) $(LFLAGS) $(CONFIG_INSTALL_Target)/lib/OpenPEARLlpc1768_board.o -I $(CONFIG_INSTALL_Target)/include $$fn.cc system.cc -o $$fn $(CONFIG_INSTALL_Target)/lib/libOpenPEARLlpc1768.a' > cc_bin.inc 

phony += run_bin
run_bin:
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
	 perl GenerateSignalDefinitions.pl GENERIC LPC1768 2>signals.log;)

../common/Signals.hcc: ../common/Signals.ods
	(cd ../common; perl GenerateSignalDefinitions.pl GENERIC LPC1768; )

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
