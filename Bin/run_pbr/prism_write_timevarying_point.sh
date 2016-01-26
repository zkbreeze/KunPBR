#!/bin/sh

n=51000
s=9
o=0.1

mkdir /Volumes/WORK_1TB/heatSphere/cut_prism_pointdata_s${s}o${o}

while [ $n -ne 60500 ]
do

	./App/PBR -u-prism-ball /Volumes/WORK_1TB/heatSphere/cut_prism_volumedata/$n.kvsml -s $s -o $o -writepoint /Volumes/WORK_1TB/heatSphere/cut_prism_pointdata_s${s}o${o}/$n.kvsml -sg

	echo "$n done"
	n=$((n+500))
done