OpenPearlStructureEN.jpg: OpenPearlStructureEN.pdf
	convert $< $@

OpenPearlStructureEN.pdf: OpenPearlStructureEN.tex
	pdflatex $<

clean:
	rm -f OpenPearlStructureEN.pdf OpenPearlStructureEN.log
	rm -f OpenPearlStructureEN.jpg OpenPearlStructureEN.aux
	
