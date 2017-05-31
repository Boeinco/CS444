all: concurrency4

concurrency4: concurrency4.o
	gcc concurrency4.o -o concurrency4 -pthread

concurrency4.o:
	gcc -c concurrency4.c -g

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg assignment4 *.o *.out *.log *.aux *~
