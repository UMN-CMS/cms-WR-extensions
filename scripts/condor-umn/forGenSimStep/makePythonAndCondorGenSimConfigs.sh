#!/bin/bash

WrMasses=(800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000)
NuMasses=( 80  160  240  320  400  600 160  320  480  640  800 1200 233  533  800 1067 1333 2000)
N_EVENTS=10000

for h in ${!WrMasses[*]}
do
	
	## MuMuJJ
	#make python config file for GEN-SIM step
	eval "sed 's/MASSWR/${WrMasses[$h]}/g ; s/MASSNU/${NuMasses[$h]}/g ; s/N_EVENTS/${N_EVENTS}/g'  WR_M-UNDEF_ToLNu_M-UNDEF_GEN-SIM_MuMuJJ.py > WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_GEN-SIM_MuMuJJ.py"    
	eval "sed 's/MASSWR/${WrMasses[$h]}/g ; s/MASSNU/${NuMasses[$h]}/g ; s/N_EVENTS/${N_EVENTS}/g'  condor_submit_WR_M-UNDEF_ToLNu_M-UNDEF_GEN-SIM_MuMuJJ.txt > condor_submit_WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_GEN-SIM_MuMuJJ.txt"    

done
