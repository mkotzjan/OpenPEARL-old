OpenPearlStructureEN.jpg: OpenPearlStructureEN.pdf
	pdfcrop $< x.pdf
	mv -f x.pdf $<
	pdf2ps $<
	echo "quit" | gs -sDEVICE=jpeg -dJPEGQ=50 -sOutputFile=$(basename $<).jpg -r600 $(basename $<).ps

OpenPearlStructureEN.pdf: OpenPearlStructureEN.tex
	pdflatex $<

clean:
	rm -f OpenPearlStructureEN.pdf OpenPearlStructureEN.log
	rm -f OpenPearlStructureEN.jpg OpenPearlStructureEN.aux
	rm -f OpenPearlStructureEN.png OpenPearlStructureEN.ps x.pdf
	
