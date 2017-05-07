all: concurrency script writeup

concurrency:
	gcc ./Concurrency2/diningPhilosophers.c -o diningPhilo -pthread

script: #copy over script to VM, run iotest.sh, put data in sectors.txt
	scp -P 8022 ./scripts/iotest.sh root@localhost:~
	ssh -p 8022 root@localhost 'bash -s' < iotest.sh 5 > ./scripts/sectors.txt
	cat sectors.txt

writeup:

