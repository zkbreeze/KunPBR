#!/bin/bash

while [ $# -ne 1 ]
do
	echo "Usage: ./unzip.sh [PARAMETER]" 
	exit 1
done

PARAMETER=$1
mkdir ${PARAMETER}

a=1
while [ $a -ne 10 ]
do
	echo unziping ${PARAMETER}f0${a}.bin.gz
	gunzip -f ${PARAMETER}f0${a}.bin.gz
	mv ${PARAMETER}f0${a}.bin ${PARAMETER}
	a=$((a+1))
done

b=10
while [ $b -ne 49 ]
do
	echo unziping ${PARAMETER}f${b}.bin.gz
	gunzip -f ${PARAMETER}f${b}.bin.gz
	mv ${PARAMETER}f${b}.bin ${PARAMETER}
	b=$((b+1))
done



