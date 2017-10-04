#DIRS = module typeFixed
DIRS = $(sort $(wildcard */))

all: splitPrlExpect
	chmod +x check.sh
	$(foreach dir,$(DIRS),cd $(dir);make;cd ..;)


clean:
	$(foreach dir,$(DIRS),cd $(dir);make clean;cd ..;)

splitPrlExpect: splitPrlExpect.c
