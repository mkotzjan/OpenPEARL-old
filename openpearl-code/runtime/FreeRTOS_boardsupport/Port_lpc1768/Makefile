include makefile.conf
include Files

CMSISINC = ../3rdparty/CMSIS/Include
INCLUDES = $(CINCS) $(CPPINCS) $(CMSISINC) 
INCS=$(addprefix -I ,$(INCLUDES))

CFLAGS += -DCORE_M3

.PHONY: all clean

all: lpc1768_boardsupport.o

startup_lpc1768.o: startup_lpc1768.S
	@echo "[assemble] startup_ARMCM3.S"
	@$(CROSS)$(CC) $^ $(CFLAGS) -c
#       @$(CROSS)$(CC) $^ $(CFLAGS) -D__NO_SYSTEM_INIT -c

cpp: tests/cpp.hex
	lpc21isp -control -term -hex $< /dev/ttyUSB0 115200 12000

firsttest: tests/firsttest.hex
	lpc21isp -control -term -hex $< /dev/ttyUSB0 115200 12000

Post.o: src/Post.cc
	$(CROSS)$(CXX) $(CXXFLAGS) $(INCS) -c -o $@ $^

lpc1768_boardsupport.o: $(CSRC) startup_lpc1768.o Post.o
	$(CROSS)$(CC) -nostdlib -Xlinker -r $(NOHOST) $(CFLAGS) $(INCS) -o $@ $^

clean:
	rm -rf *.o
