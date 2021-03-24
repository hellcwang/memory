#!/bin/bash
END=1000

while read line
do
	for i in {2..1000..50};
	do
		./dwf $i $line
	done &
done < data/name
wait
echo 'done'
