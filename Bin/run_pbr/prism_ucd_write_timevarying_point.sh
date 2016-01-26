#!/bin/sh

n=50050
s=9
o=0.1

mkdir /Volumes/WORK_1TB/heatSphere/cut_prism_pointdata_s${s}o${o}

while [ $n -ne 60050 ]
do

	./App/PBR -prism-ball /Volumes/MacPro_2TB/heatSphere/volume_ucd/$n.dat -s $s -o $o -writepoint /Volumes/WORK_1TB/heatSphere/cut_prism_pointdata_s${s}o${o}/$n.kvsml -sg

	echo "$n done"
	n=$((n+50))
done