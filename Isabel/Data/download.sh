#!/bin/bash

while [ $# -ne 1 ]
do
	echo "Usage: ./download.sh [PARAMETER]" 
	exit 1
done


PARAMETER=$1

a=1
while [ $a -ne 10 ]
do
	echo downloading http://www.vets.ucar.edu/vg/isabeldata/${PARAMETER}f0${a}.bin.gz
	curl -O http://www.vets.ucar.edu/vg/isabeldata/${PARAMETER}f0${a}.bin.gz -x proxy.kuins.net:8080

	a=$((a+1))
done

b=10
while [ $b -ne 49 ]
do
	echo downloading http://www.vets.ucar.edu/vg/isabeldata/${PARAMETER}f${b}.bin.gz
	curl -O http://www.vets.ucar.edu/vg/isabeldata/${PARAMETER}f${b}.bin.gz -x proxy.kuins.net:8080

	b=$((b+1))
done

