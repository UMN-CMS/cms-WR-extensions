#!/bin/bash

WrMasses=(2400 2400 4000 4000)
NuMasses=(160 800 1333 267)


for h in ${!WrMasses[*]}
do

	eval 'crab submit -c crabConfig_WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_RECO_MuMuJJ.py'

done
