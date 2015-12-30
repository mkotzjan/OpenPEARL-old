TEXFILES = runtime.tex changeLog.tex codeGen.tex dataTypes.tex inOutput.tex \
	intro.tex log.tex semaphore.tex signals.tex task.tex \
	literature.tex interrupt.tex\
	../linux/doc/devices.tex ../linux/doc/logx86.tex \
	../linux/doc/os.tex ../linux/doc/tasking.tex  \
	../linux/doc/interrupts.tex \
	../common/signallist.tex \
	../FreeRTOS/doc/basics.tex \
	../lpc1768/doc/basics.tex

all: runtime.pdf  $(TEXFILES)

help:
	@echo
	@echo "-----------------------------------------------------"
	@echo "make help       - produces this output"
	@echo "make            - runs pdflatex is case of change in a source file" 
	@echo "make final      - runs pdflates three times to enshure correct references"
	@echo "make clean      - removes all by make created files"   
	@echo "-----------------------------------------------------"
	@echo

final:  all
	pdflatex runtime.tex
	pdflatex runtime.tex
	pdflatex runtime.tex

runtime.pdf: $(TEXFILES)
	pdflatex runtime.tex

../common/signallist.tex: ../common/Signals.ods
	(cd ../common; perl GenerateSignalDefinitions.pl LaTeX;)

clean:
	rm -f *.log
	rm -f *.toc
	rm -f *.aux
	rm -f *.out
	rm -f *.thm
	rm -f ../common/signallist.tex
	rm -f *.pdf
