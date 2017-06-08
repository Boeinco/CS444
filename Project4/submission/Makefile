all: test_frag.c CS444_writeup4.pdf
	gcc -m32 test_frag.c -o test_frag

CS444_writeup4.pdf: CS444_writeup4.ps
	ps2pdf -dColorConversionStrategy=/LeaveColorUnchanged -dPDFSETTINGS=/printer CS444_writeup4.ps

CS444_writeup4.ps: CS444_writeup4.dvi
	dvips -t letter CS444_writeup4.dvi

CS444_writeup4.dvi: 
	latex CS444_writeup4.tex
	bibtex CS444_writeup4.tex || true
	latex CS444_writeup4.tex
	latex CS444_writeup4.tex

clean:
	rm -f *.dvi *.aux *.ps *.toc *.out *.log *.pdf
