#
# Component Makefile
#

COMPONENT_OBJS := ringbuf.o xtensa_intr.o queue.o croutine.o \
		list.o xtensa_init.o port.o event_groups.o \
		tasks.o timers.o FreeRTOS-openocd.o \
		xtensa_overlay_os_hook.o xtensa_context.o \
		portasm.o xtensa_vectors.o \
		xtensa_vector_defaults.o xtensa_intr_asm.o \
		addOns/assert.o addOns/clock.o \
		addOns/FreeRTOSClock.o \
		addOns/FreeRTOSHooks.o \
		addOns/timer.o \
		addOns/service.o \
		PEARL/CSema.o PEARL/Mutex.o \
		PEARL/PrioMapper.o PEARL/Task.o \
		PEARL/TaskMonitor.o PEARL/TaskTimer.o

COMPONENT_SRCDIRS := . addOns PEARL
COMPONENT_ADD_LDFLAGS += -Wl,--undefined=uxTopUsedPriority
COMPONENT_ADD_INCLUDEDIRS := include addOns PEARL
COMPONENT_PRIV_INCLUDEDIRS := include/freertos ../../../../common
CXXFLAGS += -frtti

# Overriding build and clean to add mkFakeTypes
COMPONENT_OWNBUILDTARGET := 1
COMPONENT_OWNCLEANTARGET := 1

.PHONY: build
build: mkFakeTypes $(COMPONENT_LIBRARY)

mkFakeTypes: 
	@echo "[create] $@"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(addprefix -I ,$(COMPONENT_INCLUDES)) $(addprefix -I ,$(COMPONENT_EXTRA_INCLUDES)) -I $(COMPONENT_PATH)/addOns -c $(COMPONENT_PATH)/addOns/mkFakeTypes.c
	@mv mkFakeTypes.o $(COMPONENT_PATH)/addOns/
	@(cd $(COMPONENT_PATH)/addOns; size -A mkFakeTypes.o >sizes.txt;\
	 gcc fakeTypes.c -o fakeTypes; ./fakeTypes)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(addprefix -I ,$(COMPONENT_INCLUDES)) $(addprefix -I ,$(COMPONENT_EXTRA_INCLUDES)) -I $(COMPONENT_PATH)/addOns -c $(COMPONENT_PATH)/addOns/check_FakeTypes.c -o $(COMPONENT_PATH)/addOns/check_FakeTypes.o
	@(cd $(COMPONENT_PATH)/addOns; \
	rm -f sizes.txt mkFakeTypes.o check_FakeTypes.o fakeTypes)

task:
	@echo "[create] $@ with -frtti"
	@echo "CXX build/freertos/PEARL/Task.o"
	$(CXX) $(CXXFLAGS) -frtti $(CPPFLAGS) $(addprefix -I ,$(COMPONENT_INCLUDES)) $(addprefix -I ,$(COMPONENT_EXTRA_INCLUDES)) -I PEARL/ -c $(COMPONENT_PATH)/PEARL/Task.cc -o PEARL/Task.o
	#$(call AppendSourceToDependencies,$$<,$$@)
	

# Build the archive. We remove the archive first, otherwise ar will get confused if we update
# an archive when multiple filenames have the same name (src1/test.o and src2/test.o)
$(COMPONENT_LIBRARY): $(COMPONENT_OBJS)
	$(summary) AR $(patsubst $(PWD)/%,%,$(CURDIR))/$@
	rm -f $@
	$(AR) cru $@ $^


CLEAN_FILES := $(COMPONENT_LIBRARY) $(COMPONENT_OBJS) $(COMPONENT_OBJS:.o=.d) $(COMPONENT_EXTRA_CLEAN) component_project_vars.mk
.PHONY: clean
clean:
	$(summary) RM $(CLEAN_FILES)
	rm -f $(CLEAN_FILES)
