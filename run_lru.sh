#!/bin/bash
END=1000

while read line
do
	for i in {2..100..1};
	do
		echo $line
		./total_lru $i $line
	done &
done < total_lru_data/name
wait
while read line
do
	for i in {100..1000..20};
	do
		echo $line
		./total_lru $i $line
	done &
done < total_lru_data/name
wait

echo 'done'
