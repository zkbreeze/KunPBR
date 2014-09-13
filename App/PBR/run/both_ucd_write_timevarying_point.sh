#!/bin/sh

n=50050
s=16
o=0.3

mkdir /Volumes/WORK_1TB/heatSphere/both_pointdata_s${s}o${o}

while [ $n -ne 60050 ]
do

	./PBR -both /Volumes/MacPro_2TB/heatSphere/volume_ucd/$n.dat -s $s -o $o -trans tfunc_both_a.kvsml -writepoint /Volumes/WORK_1TB/heatSphere/both_pointdata_s${s}o${o}/$n.kvsml -sg 

	echo "$n done"
	n=$((n+50))
done