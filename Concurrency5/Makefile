all: concurrency5

concurrency5: concurrency5.o
	gcc concurrency5.o -o concurrency5 -pthread

concurrency5.o:
	gcc -c concurrency5.c -g

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg concurrency5 *.o *.out *.log *.aux *~
