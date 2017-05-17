#!/bin/bash

WrMasses=(800 1600 2400 3200 4000 6000) 
NuMasses=(400 800 1200 1600 2000 3000)  

inputDatasetsEE=('/WRToEEJJ_MW-800_MNu-400_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToEEJJ_MW-1600_MNu-800_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToEEJJ_MW-2400_MNu-1200_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToEEJJ_MW-3200_MNu-1600_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToEEJJ_MW-4000_MNu-2000_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToEEJJ_MW-6000_MNu-3000_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER')

inputDatasetsMuMu=('/WRToMuMuJJ_MW-800_MNu-400_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToMuMuJJ_MW-1600_MNu-800_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToMuMuJJ_MW-2400_MNu-1200_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToMuMuJJ_MW-3200_MNu-1600_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToMuMuJJ_MW-4000_MNu-2000_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER'
	'/WRToMuMuJJ_MW-6000_MNu-3000_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_mcRun2_asymptotic_2016_miniAODv2_v1_AODSIM-ba893ef795de00e40a7e309335416736/USER')


for h in ${!WrMasses[*]}
do

	## EEJJ
	#make python config file for miniAOD step 
	eval "sed 's/MASSWR/${WrMasses[$h]}/g' WR_M-UNDEF_ToLNu_M-UNDEF_miniAOD.py > WR_temp1.py"                        
	eval "sed 's/MASSNU/${NuMasses[$h]}/g'  WR_temp1.py > WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_miniAOD_EEJJ.py"
	rm WR_temp1.py 

	#make crab config file
	eval "sed 's/WR_M-UNDEF/WR_M-${WrMasses[$h]}/g' crabConfig_WR_M-UNDEF_ToLNu_M-UNDEF_miniAOD_EEJJ.py > crabConfig_WR_temp1.py"
	eval "sed 's/ToLNu_M-UNDEF/ToLNu_M-${NuMasses[$h]}/g' crabConfig_WR_temp1.py > crabConfig_WR_temp2.py"
	eval "sed 's@datasetFromDBS@${inputDatasetsEE[$h]}@g' crabConfig_WR_temp2.py > crabConfig_WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_miniAOD_EEJJ.py"
	rm crabConfig_WR_temp1.py crabConfig_WR_temp2.py 


	## MuMuJJ
	#make python config file for miniAOD step 
	eval "sed 's/MASSWR/${WrMasses[$h]}/g' WR_M-UNDEF_ToLNu_M-UNDEF_miniAOD.py > WR_temp1.py"                        
	eval "sed 's/MASSNU/${NuMasses[$h]}/g'  WR_temp1.py > WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_miniAOD_MuMuJJ.py"
	rm WR_temp1.py 

	#make crab config file
	eval "sed 's/WR_M-UNDEF/WR_M-${WrMasses[$h]}/g' crabConfig_WR_M-UNDEF_ToLNu_M-UNDEF_miniAOD_MuMuJJ.py > crabConfig_WR_temp1.py"
	eval "sed 's/ToLNu_M-UNDEF/ToLNu_M-${NuMasses[$h]}/g' crabConfig_WR_temp1.py > crabConfig_WR_temp2.py"
 	eval "sed 's@datasetFromDBS@${inputDatasetsMuMu[$h]}@g' crabConfig_WR_temp2.py > crabConfig_WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_miniAOD_MuMuJJ.py"
	rm crabConfig_WR_temp1.py crabConfig_WR_temp2.py 

done