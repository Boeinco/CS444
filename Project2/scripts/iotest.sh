#!/bin/bash

num_trees=$1

rm -rf testdir

num_write=0

sectors=()
fmt=("\t"$num_write"\n")

sectors+=$(du ~ | tail -n1 | awk '{print $1}' 2>&1)
fmt=("\t\t"$num_write"\n")
sectors+=$fmt

mkdir testdir
cd testdir

sectors+=$(du ~ | tail -n1 | awk '{print $1}' 2>&1)
num_write=$((num_write + 1))
fmt=("\t\t"$num_write"\n")
sectors+=$fmt

for i in `seq 0 $num_trees`; do
	mkdir test$i
	cd test$i
	for j in `seq 0 $num_trees`; do
		cat ~/hamlet.txt > test"$j".txt #copies the full text of Hamlet
		sectors+=$(du ~ | tail -n1 | awk '{print $1}' 2>&1)
		num_write=$(($num_write + 1))
		fmt=("\t\t"$num_write"\n")
		sectors+=$fmt

	done
	cd ..
done

cd ..

rm -rf testdir

sectors+=$(du ~ | tail -n1 | awk '{print $1}' 2>&1)
num_write=$(($num_write + 1))
fmt=("\t\t"$num_write"\n")
sectors+=$fmt

echo -e "Sectors Used \t I/O Operations" > sectors.txt
echo -e $sectors >> sectors.txt

cat sectors.txt
