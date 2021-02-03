END=1000

while read line
do
	for i in $(seq 2 $END);
	do
		./dwf $i $line
	done &
done < data/name
