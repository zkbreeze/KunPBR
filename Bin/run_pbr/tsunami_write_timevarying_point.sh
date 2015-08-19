#!/bin/sh

n=100

while [ $n -ne 120 ]
do
	./PBR -tsunami /Volumes/HD-PNFU3/130402/ishi_city_r05/data/out_00${n}.posivelo -writepoint out_00${n}.kvsml

	echo "$n done"
	n=$((n+1))
done

