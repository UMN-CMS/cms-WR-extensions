#!/bin/bash

WrMasses=(800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000)
NuMasses=( 80  160  240  320  400  600 160  320  480  640  800 1200 233  533  800 1067 1333 2000)
DataSetsDirectory="/data/whybee0b/user/aevans/"

for h in ${!WrMasses[*]}
do
	
	## MuMuJJ
	#make python config file for HLT step
	eval "sed 's/MASSWR/${WrMasses[$h]}/g ; s/MASSNU/${NuMasses[$h]}/g ; s/N_EVENTS/${N_EVENTS}/g'  WR_M-UNDEF_ToLNu_M-UNDEF_RECO_MuMuJJ_cfg.py > WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_RECO_MuMuJJ_cfg.py"    
    eval "ls ${DataSetsDirectory}/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_HLT_MuMuJJ/*000.root > WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_HLT_MuMuJJ.txt"

    eval 'condor_filelist.perl --batch 20 --nosubmit WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_RECO_MuMuJJ_cfg.py WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_HLT_MuMuJJ.txt --memreq 2500'

done
