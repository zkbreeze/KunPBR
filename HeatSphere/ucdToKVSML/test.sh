#!/bin/sh

NUMBER=50500

while [ $NUMBER -le 60001 ]
do
#/home/vizlab/Dropbox/program/2014/08/kvsmlmake/kvsmlmake -tetra /home/vizlab/0723data/$NUMBER.dat  -s 6  -output /home/vizlab/kvsmldata/choice_rep6/$NUMBER.kvsml -trans /home/vizlab/Dropbox/program/2014/08/transferfunction1 -nos
/home/vizlab/Dropbox/program/2014/08/kvsmlmake_kun/PBR_kun -tetra /home/vizlab/0723data/$NUMBER.dat  -s 3  -output /home/vizlab/kvsmldata/kun_tetra_s3_o0.1/$NUMBER.kvsml -o 0.05 -nos
echo "$NUMBER done"
NUMBER=$((NUMBER+500))
done