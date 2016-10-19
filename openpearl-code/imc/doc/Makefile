TEXFILES = imc.tex \
	intro.tex structure_system_build.tex\
	structure_import_export.tex \
	plattform_xml.tex usage.tex  \
	documentation.tex \
	literature.tex

all: imc.pdf  $(TEXFILES)

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
	pdflatex imc.tex
	pdflatex imc.tex
	pdflatex imc.tex

imc.pdf: $(TEXFILES)
	pdflatex imc.tex

clean:
	rm -f *.log
	rm -f *.toc
	rm -f *.aux
	rm -f *.out
	rm -f *.thm
	rm -f *.pdf
