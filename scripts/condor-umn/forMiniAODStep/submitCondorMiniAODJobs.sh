#!/bin/bash

WrMasses=(800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000)
NuMasses=( 80  160  240  320  400  600 160  320  480  640  800 1200 233  533  800 1067 1333 2000)


for h in ${!WrMasses[*]}
do

	eval 'condor_submit condor_submit_WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_MiniAOD_MuMuJJ.txt'

done
