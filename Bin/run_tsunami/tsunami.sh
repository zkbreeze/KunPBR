#!/bin/sh

# run_tsunami
# Default Mode
# ./Tsunami -f ~/Data/tsunami/particle/out_00125.kvsml -rep 81 -m 200 -l ~/Data/tsunami/structure.obj

# Clipping Mode
./Tsunami -f ~/Data/tsunami/particle/out_00125.kvsml -rep 81 -m 200 -l ~/Data/tsunami/structure.obj -minx 1400 -miny 1500 -minz 1.0 -maxx 1700 -maxy 2100 -maxz 13.0