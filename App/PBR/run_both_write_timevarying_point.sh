#!/bin/sh

n=51000

while [ $n -ne 60500 ]
do

	./PBR -both /Volumes/WORK_1TB/heatSphere/140421/ucd/all/$n.dat -s 6 -o 0.1 -writepoint /Volumes/WORK_1TB/heatSphere/both_pointdata_s6o0.1/$n.kvsml

	echo "$n done"
	n=$((n+500))
done