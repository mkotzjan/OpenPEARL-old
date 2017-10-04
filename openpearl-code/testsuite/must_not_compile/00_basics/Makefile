TARGETS = $(shell ls *.prl)

all:
	@cat README
	@../check.sh .. $(TARGETS)


clean:
	$(foreach tgt,$(TARGETS),rm -f  $(basename $(tgt)))
	rm -f *.log *.err *.xml *.cc 
