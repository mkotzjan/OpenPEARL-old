TEXFILES = OpenPEARL_linux.tex ../../common/signallist.tex

all: OpenPEARL_linux.pdf  $(TEXFILES)

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
	pdflatex OpenPEARL_linux.tex
	pdflatex OpenPEARL_linux.tex
	pdflatex OpenPEARL_linux.tex

OpenPEARL_linux.pdf: $(TEXFILES)
	pdflatex OpenPEARL_linux.tex

../../common/signallist.tex: ../../common/Signals.ods
	(cd ../../common; perl GenerateSignalDefinitions.pl LaTeX;)

clean:
	rm -f *.log
	rm -f *.toc
	rm -f *.aux
	rm -f *.out
	rm -f *.thm
	rm -f ../../common/signallist.tex
	rm -f *.pdf
