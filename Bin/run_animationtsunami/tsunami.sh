#!/bin/sh

# run_animationtsunami
# Default Mode 
./App/AnimationTsunami -f /Users/breeze/Data/tsunami/TimeVaryingParticle/ -l /Users/breeze/Data/tsunami/structure.obj -rep 81 -m 200 

# Clipping Mode
# ./App/AnimationTsunami -f /Users/breeze/Data/tsunami/TimeVaryingParticle/ -l /Users/breeze/Data/tsunami/structure.obj -rep 81 -m 200 -minx 1400 -miny 1500 -minz 1.0 -maxx 1700 -maxy 2100 -maxz 13.0