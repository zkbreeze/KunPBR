#!/bin/sh

n=51000
mkdir /Volumes/WORK_1TB/heatSphere/both_pointdata_binary

while [ $n -ne 60500 ]
do

	./PBR -point /Volumes/WORK_1TB/heatSphere/both_pointdata/$n.kvsml -writepoint /Volumes/WORK_1TB/heatSphere/both_pointdata_binary/$n.kvsml

	echo "$n done"
	n=$((n+500))
done