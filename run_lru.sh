#!/bin/bash
END=1000

while read line
do
	for i in {2..1000..50};
	do
		echo $line
		./total_lru $i $line
	done &
done < total_lru_data/name
wait
