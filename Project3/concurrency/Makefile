all: concurrency3

concurrency3: concurrency3.o
	gcc concurrency3.o -o concurrency3 -pthread

concurrency3.o:
	gcc -c concurrency3.c -g

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg assignment3 *.o *.out *.log *.aux *~
