
MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(dir $(MAKEFILE_PATH))

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
LINK = @echo [link] $@; $(CXX) $(PURECFLAGS) -DTARGET=3 $(CXXFLAGS) $(CPPFLAGS) $(LFLAGS) $(LDFLAGS) $^ -o $@

IC_DIR = ../includeComposer
IC = $(IC_DIR)/ic
DWS = $(IC_DIR)/dws

INCS = .
CXXSRCS =
CRCS =

#CHAN_FS= ../FreeRTOS/chanFatFs/src
#LPCOPEN_FS= ../cortexM/LPCOpen/software/filesystems

targets := esp32.xml

# -------------------- add CMSIS support to project
#INCS += ../cortexM/CMSIS/Include

# -------------------- add FreeRTOS support to project
#include esp-idf/components/freertos/Files
#CSRC += $(addprefix esp-idf/components/freertos/,$(RTOSCSRC))
#INCS += $(addprefix esp-idf/components/freertos/,$(RTOSINCS))

# -------------------- add FreeRTOS addOn files to project
#include esp-idf/components/freertos/addOns/Files
#CSRC += $(addprefix esp-idf/components/freertos/addOns/,$(ADDONCSRC))
#CHECK_FILES += $(addprefix esp-idf/components/freertos/addOns/,$(ADDONCSRC))
#INCS += $(addprefix esp-idf/components/freertos/addOns/,$(ADDONINCS))

# ------------------- add FreeRTOS specific PEARL classes to project
include esp-idf/components/freertos/PEARL/Files
#CXXSRCS += $(addprefix esp-idf/components/freertos/PEARL/,$(PEARL_FREERTOS_SRC))
#CHECK_FILES += $(addprefix esp-idf/components/freertos/PEARL/,$(PEARL_FREERTOS_SRC))
#CHECK_FILES += $(addprefix esp-idf/components/freertos/PEARL/,$(addsuffix .h,$(basename $(PEARL_FREERTOS_SRC))))
#INCS += $(addprefix esp-idf/components/freertos/PEARL/,$(PEARL_FREERTOS_INC))
XMLFILES += $(addprefix esp-idf/components/freertos/PEARL/,$(PEARL_FREERTOS_XML))

# -------------------- add cortex-M clock to project
#CXXSRCS += ../FreeRTOS/addOns/CortexMClock.cc
#CHECK_FILES += ../FreeRTOS/addOns/CortexMClock.cc

# ------------------- add plattform independent PEARL classes to project
include ../common/Files.common
#CXXSRCS += $(addprefix ../common/,$(CXX_COMMON))
#CHECK_FILES += $(addprefix ../common/,$(CXX_COMMON))
#INCS += ../common
XMLFILES  += $(addprefix ../common/,$(XML_COMMON))

# -------------------- add includes for the include composer
INCS += esp-idf/components/app_trace/include
INCS += esp-idf/components/app_update/include
INCS += esp-idf/components/boardsupport/include
INCS += esp-idf/components/bootloader_support/include
INCS += esp-idf/components/bt/include
INCS += esp-idf/components/coap/port/include
INCS += esp-idf/components/coap/port/include/coap
INCS += esp-idf/components/coap/libcoap/include
INCS += esp-idf/components/coap/libcoap/include/coap
INCS += esp-idf/components/console/.
INCS += esp-idf/components/cxx/include
INCS += esp-idf/components/driver/include
INCS += esp-idf/components/esp32/include
INCS += esp-idf/components/esp_adc_cal/include
INCS += esp-idf/components/ethernet/include
INCS += esp-idf/components/expat/port/include
INCS += esp-idf/components/expat/include/expat
INCS += esp-idf/components/fatfs/src
INCS += esp-idf/components/freertos/include
INCS += esp-idf/components/freertos/addOns
INCS += esp-idf/components/freertos/PEARL
INCS += esp-idf/components/heap/include
INCS += esp-idf/components/idf_test/include
INCS += esp-idf/components/jsmn/include/
INCS += esp-idf/components/json/cJSON
INCS += esp-idf/components/libsodium/libsodium/src/libsodium/include
INCS += esp-idf/components/libsodium/port_include
INCS += esp-idf/components/log/include
INCS += esp-idf/components/lwip/include/lwip
INCS += esp-idf/components/lwip/include/lwip/port
INCS += esp-idf/components/lwip/include/lwip/posix
INCS += esp-idf/components/lwip/apps/ping
INCS += esp-idf-template/main/include
INCS += esp-idf/components/mbedtls/port/include
INCS += esp-idf/components/mbedtls/include
INCS += esp-idf/components/mdns/include
INCS += esp-idf/components/micro-ecc/micro-ecc
INCS += esp-idf/components/newlib/platform_include
INCS += esp-idf/components/newlib/include
INCS += esp-idf/components/nghttp/port/include
INCS += esp-idf/components/nghttp/nghttp2/lib/includes
INCS += esp-idf/components/nvs_flash/include
INCS += ../common
INCS += esp-idf/components/openssl/include
INCS += esp-idf/components/pthread/include
INCS += esp-idf/components/sdmmc/include
INCS += esp-idf/components/soc/esp32/include
INCS += esp-idf/components/soc/include
INCS += esp-idf/components/spi_flash/include
INCS += esp-idf/components/spiffs/include
INCS += esp-idf/components/tcpip_adapter/include
INCS += esp-idf/components/ulp/include
INCS += esp-idf/components/vfs/include
INCS += esp-idf/components/wear_levelling/include
INCS += esp-idf/components/wpa_supplicant/include
INCS += esp-idf/components/wpa_supplicant/port/include
INCS += esp-idf/components/wpa_supplicant/../esp32/include
INCS += esp-idf/components/xtensa-debug-module/include

# -------------------- add boardsupport component xml files
#XMLFILES += esp-idf/components/boardsupport/Esp32Uart.xml
XMLFILES += esp-idf/components/boardsupport/StdOut.xml
XMLFILES += esp-idf/components/boardsupport/Esp32BME280.xml
XMLFILES += esp-idf/components/boardsupport/Esp32Wifi.xml
XMLFILES += esp-idf/components/boardsupport/TcpIpServer.xml

define esp32build
make -C esp-idf-template
endef

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
INCS:=$(addprefix -I ,$(INCS))
PURECXXFLAGS := $(CXXFLAGS)
#ifeq ($(CONFIG_LPC1768_CHECK_STACK_OVERFLOW),y)
#PURECXXFLAGS += -finstrument-functions 
#endif

PURECFLAGS := $(CFLAGS)
CFLAGS += $(INCS)
CXXFLAGS += $(INCS)
CXXFLAGS += -DTARGET=3

# ----------------- all input sources for dependencies
#SRCS = $(BOARDSUPPORT)  $(CXXSRCS) $(CSRC)

# ------------------- all files not to be added into lib
#BOARDSUPPORT_OBJ := \
#		 $(addsuffix .o,$(basename $(BOARDSUPPORT))) \
#		 $(addsuffix .o,$(basename $(LPCOpen_CSRC)))

#INCLUDES := $(addsuffix .h,$(basename $(CXXSRCS)))
#PLIB_OBJS +=  $(addsuffix .o,$(basename $(LPCOpen_CSRC)))
#PLIB_OBJS := $(addsuffix .o,$(basename $(CXXSRCS)))
#PLIB_OBJS += $(addsuffix .o,$(basename $(CSRC)))


CHECK_FILES := $(sort $(CHECK_FILES))

#$(warning BOARDSUPPORT_OBJ=$(BOARDSUPPORT_OBJ))
#$(warning XMLFILES=$(XMLFILES))

all: esp-idf-build $(targets)

tests/testStackOverflow.s: tests/testStackOverflow.cc
	$(CXX) $^ -S $(CXXFLAGS)  $(PURECXXFLAGS)

esp-idf-build:
	@echo "[create] $@"
	#export IDF_PATH=$(CURRENT_DIR)esp-idf
	$(call esp32build)

#libOpenPEARLesp32.a: esp-idf-build $(PLIB_OBJS)
#	@echo "[create] $@"
#	@rm   -f libOpenPEARLesp32.a
#	@rm   -f libOpenPEARLesp32complete.a
#	@rm   -f libOpenPEARLesp32complete.mri
#	@$(AR) r libOpenPEARLesp32.a $(PLIB_OBJS)
#	@echo "create libOpenPEARLesp32complete.a" > libOpenPEARLesp32complete.mri
#	@echo "addlib libOpenPEARLesp32.a" >> libOpenPEARLesp32complete.mri
#	@find esp-idf-template/build/ -maxdepth 2 -regex ".*\.a" | grep -v main | grep -v freertos | sed -e 's/^/addlib /' >> libOpenPEARLesp32complete.mri
#	# Add component libraries according to specific component.mk files
#	@echo "addlib esp-idf/components/newlib/lib/libc.a" >> libOpenPEARLesp32complete.mri
#	@echo "addlib esp-idf/components/newlib/lib/libm.a" >> libOpenPEARLesp32complete.mri
#	@echo "addlib esp-idf/components/bt/lib/libbtdm_app.a" >> libOpenPEARLesp32complete.mri
#	@echo "addlib esp-idf/components/esp32/libhal.a" >> libOpenPEARLesp32complete.mri
#	@find esp-idf/components/esp32/lib -regex ".*\.a" | sed -e 's/^/addlib /' >> libOpenPEARLesp32complete.mri
#	@echo "save\nend" >> libOpenPEARLesp32complete.mri
#	@$(AR) -M <libOpenPEARLesp32complete.mri
	#@mv   -f libOpenPEARLesp32complete.a libOpenPEARLesp32.a

esp32.xml: $(XMLFILES)
	@echo "[create] $@"
	#echo $(XMLFILES)
	@echo "[build] $@"
	@echo "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" > $@
	@echo "<platform file=\"$@\">" >> $@
	@cat $(XMLFILES) >> $@
	@echo "</platform>" >> $@


#phony += depend
#depend: ../common/Signals.hh ../common/Signals.hcc $(SRCS)
#	@echo "[create] $@"
#	@$(CXX) -MM $(CFLAGS) $(CPPFLAGS) $(INCS) $^ >.depend

phony += clean
clean:
	$(call esp32build) clean
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
	@mkdir -p $(CONFIG_INSTALL_Target)/lib/OpenPEARLesp32
	chmod 777 $(CONFIG_INSTALL_Target)/lib/OpenPEARLesp32
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
	#Find and copy all generated esp32 libraries
	@find esp-idf-template/build/ -maxdepth 2 -regex ".*\.a" -exec cp {} $(CONFIG_INSTALL_Target)/lib/OpenPEARLesp32 \;
	@cp esp-idf-template/build/bootloader/bootloader.bin $(CONFIG_INSTALL_Target)/lib/OpenPEARLesp32
	@mkdir -p $(CONFIG_INSTALL_Target)/lib/ldscripts
	@cp OpenPEARLesp32.ld $(CONFIG_INSTALL_Target)/lib/ldscripts
	@cp esp32.xml $(CONFIG_INSTALL_Target)/lib

####### TODO
phony += cc_bin
cc_bin:
	@echo "[create] $@"
	@echo '$(CXX) -g -DTARGET=3 -nostdlib -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,-static -Wl,--start-group -mlongcalls -frtti -u call_user_start_cpu0 -Os -I $(CONFIG_INSTALL_Target)/include $$SYSTEM $$fn.cc -L/usr/local/lib/OpenPEARLesp32 -lapp_trace -lapp_update -laws_iot -lboardsupport -lbootloader_support -lbt -lcoap -lconsole -lcxx -u __cxa_guard_dummy -ldriver -lesp32 $(CURRENT_DIR)/esp-idf/components/esp32/libhal.a -L$(CURRENT_DIR)/esp-idf/components/esp32/lib -lcore -lrtc -lnet80211 -lpp -lwpa -lsmartconfig -lcoexist -lwps -lwpa2 -lespnow -lphy -L$(CURRENT_DIR)/esp-idf/components/esp32/ld -L$(CURRENT_DIR)/esp-idf-template/build/esp32 -T esp32_out.ld -u ld_include_panic_highint_hdl -T esp32.common.ld -T esp32.rom.ld -T esp32.peripherals.ld -T esp32.rom.spiram_incompatible_fns.ld -lesp_adc_cal -lethernet -lexpat -lfatfs -lfreertos -Wl,--undefined=uxTopUsedPriority -lheap -lidf_test -ljsmn -ljson -llibsodium -llog -llwip -lmbedtls -lmdns -lmicro-ecc $(CURRENT_DIR)/esp-idf/components/newlib/lib/libc.a $(CURRENT_DIR)/esp-idf/components/newlib/lib/libm.a -lnewlib -lnghttp -lnvs_flash -lopenpearl_common -lopenssl -lpthread -lsdmmc -lsoc -lspi_flash -lspiffs -ltcpip_adapter -lulp -lvfs -lwear_levelling -lwpa_supplicant -lxtensa-debug-module -lgcc -lstdc++ -lgcov -Wl,--end-group -Wl,-EL -o $$fn -Wl,-Map=esp32.map' >> cc_bin.inc
	@echo 'python $(CURRENT_DIR)/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "2MB"  -o $$fn.bin $$fn' >> cc_bin.inc

######## TODO
phony += run_bin
run_bin:
	@echo "[create] $@"
	@echo "# auto generated " > run_bin.inc
	@echo 'python $(CURRENT_DIR)/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB1 --baud 115200 --before default_reset --after hard_reset write_flash -u --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 $(CONFIG_INSTALL_Target)/lib/OpenPEARLesp32/bootloader.bin 0x10000 $$fn.bin 0x8000 $(CURRENT_DIR)/esp-idf-template/build/partitions_singleapp.bin' >>run_bin.inc
	@echo 'python -m serial.tools.miniterm --rts 0 --dtr 0 --raw /dev/ttyUSB1 115200' >>run_bin.inc

# -------------------------------------------------
# additional rules to enable make to build the autogenerated files
#
#../common/Signals.hh: ../common/Signals.ods
#	@echo create signal definitions from spreadsheet
#	@(cd ../common; \
#	 perl GenerateSignalDefinitions.pl GENERIC ESP32 2>signals.log;)

#../common/Signals.hcc: ../common/Signals.ods
#	(cd ../common; perl GenerateSignalDefinitions.pl GENERIC ESP32; )

#../Signals.cc: ../Signals.hh ../Signals.hcc

phony +=  checkformat
checkformat:
	@$(foreach f,$(CHECK_FILES),../codestyle/check.sh $(f);)

#phony += mkFakeTypes
#mkFakeTypes: 
#	@echo "[create] $@"
#	@$(CC) $(INCS) -c esp-idf/components/freertos/addOns/mkFakeTypes.c
#	@mv mkFakeTypes.o esp-idf/components/freertos/addOns/
#	@(cd esp-idf/components/freertos/addOns; size -A mkFakeTypes.o >sizes.txt;\
#	 gcc fakeTypes.c -o fakeTypes; ./fakeTypes)
#	@$(CXX) $(INCS) -c esp-idf/components/freertos/addOns/check_FakeTypes.c
#	@(cd esp-idf/components/freertos/addOns; \
#	rm -f sizes.txt mkFakeTypes.o check_FakeTypes.o fakeTypes)

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

print-%  : ; @echo $* = $($*)
