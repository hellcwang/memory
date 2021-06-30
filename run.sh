#!/bin/bash
END=1000

while read line
do
	for i in {100..1000..20};
	do
		./dwf $i $line
	done &
done < data/name
wait
echo 'done'
