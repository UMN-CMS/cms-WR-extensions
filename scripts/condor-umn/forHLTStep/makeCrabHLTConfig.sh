#!/bin/bash

WrMasses=(2400 2400 4000 4000) 
NuMasses=(160 800 1333 267)  

inputDatasetsMuMu=('/WRToMuMuJJ_MW-2400_MNu-160_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-e2ebad1b90caaf241f3a6c8cb5bb0f15/USER'
                   '/WRToMuMuJJ_MW-2400_MNu-800_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-e2ba59231ce4b9003c4220129f010e56/USER'
                   '/WRToMuMuJJ_MW-4000_MNu-1333_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-60196d25a929b65822cbc558e3b0efe7/USER'
                   '/WRToMuMuJJ_MW-4000_MNu-267_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-d5aafc3bc7642aee780d5739180b0832/USER')

for h in ${!WrMasses[*]}
do
	## MuMuJJ
	#make python config file for HLT step 
	eval "sed 's/MASSWR/${WrMasses[$h]}/g' WR_M-UNDEF_ToLNu_M-UNDEF_HLT.py > WR_temp1.py"                        
	eval "sed 's/MASSNU/${NuMasses[$h]}/g'  WR_temp1.py > WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_HLT_MuMuJJ.py"
	rm WR_temp1.py 

	#make crab config file
	eval "sed 's/WR_M-UNDEF/WR_M-${WrMasses[$h]}/g' crabConfig_WR_M-UNDEF_ToLNu_M-UNDEF_HLT_MuMuJJ.py > crabConfig_WR_temp1.py"
	eval "sed 's/ToLNu_M-UNDEF/ToLNu_M-${NuMasses[$h]}/g' crabConfig_WR_temp1.py > crabConfig_WR_temp2.py"
 	eval "sed 's@datasetFromDBS@${inputDatasetsMuMu[$h]}@g' crabConfig_WR_temp2.py > crabConfig_WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_HLT_MuMuJJ.py"
	rm crabConfig_WR_temp1.py crabConfig_WR_temp2.py 

done
