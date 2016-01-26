#!/bin/bash

while [ $# -ne 1 ]
do
	echo "Usage: ./~.sh [PARAMETER]" 
	exit 1
done


PARAMETER=$1
s=3
o=0.1
mkdir /Volumes/WORK_1TB/Isabel/point/${PARAMETER}
mkdir /Volumes/WORK_1TB/Isabel/point/${PARAMETER}/point_s${s}o${o}

a=1
while [ $a -ne 10 ]
do
	./App/PBR -k /Volumes/WORK_1TB/Isabel/volume/${PARAMETER}/${PARAMETER}f0${a}.kvsml -s $s -o $o -writepoint /Volumes/WORK_1TB/Isabel/point/${PARAMETER}/point_s${s}o${o}/${PARAMETER}f0${a}.kvsml
	a=$((a+1))
done

b=10
while [ $b -ne 49 ]
do
	./App/PBR -k /Volumes/WORK_1TB/Isabel/volume/${PARAMETER}/${PARAMETER}f${b}.kvsml -s $s -o $o -writepoint /Volumes/WORK_1TB/Isabel/point/${PARAMETER}/point_s${s}o${o}/${PARAMETER}f${b}.kvsml
	b=$((b+1))
done