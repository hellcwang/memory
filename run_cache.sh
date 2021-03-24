#!/bin/bash
END=1000

while read line
do
	for i in {2..1000..50};
	do
		echo $line $i
		./cache $i $line
	done &
done < data_cache/name

#reference https://stackoverflow.com/questions/6511884/how-to-limit-number-of-threads-sub-processes-used-in-a-function-in-bash
