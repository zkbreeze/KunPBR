#!/bin/sh

# run_tsunami
# Default Mode
# ./Tsunami -adv ~/Data/1f_in/r02/output_advone_00050.adv -rep 81 -l ~/Data/1f_in/151125/f1_inside_20141126/f1_inside_20141126.obj
./Tsunami -adv ~/Data/1f_in/r02/output_advone_00050.adv -v ~/Data/1f_in/f1_inside_20141126.vtk -o 0.1 -rep 200

# Clipping Mode
# ./Tsunami -f ~/Data/tsunami/particle/out_00125.kvsml -rep 81 -m 200 -l ~/Data/tsunami/structure.obj -minx 1400 -miny 1500 -minz 1.0 -maxx 1700 -maxy 2100 -maxz 13.0