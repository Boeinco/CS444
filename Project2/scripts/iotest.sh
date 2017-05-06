#!/bin/bash

rm -rf testdir

mkdir testdir
cd testdir

for i in `seq 0 8`; do
	mkdir test$i
	cd test$i
	for j in `seq 0 8`; do
		echo "Isaac is the best!" > test"$j".txt
		du ~/testdir | tail -n1 | awk '{print $1}'
	done
	cd ..
done

for i in `seq 0 8`; do
	cd test$i
	for j in `seq 0 8`; do
		filename=test"$i".txt

		B=$(hdparm --fibmap $filename | grep 'sectors' -A 1 | tail -n1 | awk '{print $4}')
		fmt=" has size "
		echo $filename$fmt$B
	done
	cd ..
done
