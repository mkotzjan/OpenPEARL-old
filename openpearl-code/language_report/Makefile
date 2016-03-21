TEXFILES = 01-introduction.tex 		\
	02-fundamentals.tex 		\
	03-language_forms.tex		\
	04-program_structure.tex	\
	05-variables_and_constants.tex	\
	06-expressions.tex		\
	07-sequential.tex		\
	08-procedures.tex		\
	09-parallel_activities.tex	\
	10-input_and_output.tex		\
	11-signals.tex			\
	90-appendix.tex			\
	macrosShowChanges.tex		\
	macrosShowResult.tex

.PHONY: clean final
include ../configuration/.config

ifeq ($(CONFIG_DOC_PREPORT),y)

p-report.pdf:	p-report.tex $(TEXFILES)
	pdflatex p-report.tex

final:
	pdflatex p-report.tex
	pdflatex p-report.tex
	pdflatex p-report.tex
endif

clean:
	rm -rf p-report.cb 
	rm -rf p-report.cb2
	rm -rf p-report.log
	rm -rf p-report.out
	rm -rf p-report.toc
	rm -rf p-report.aux

