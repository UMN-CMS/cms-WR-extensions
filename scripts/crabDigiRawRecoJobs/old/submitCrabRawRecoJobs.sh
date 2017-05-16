#!/bin/bash
WrMasses=(2400 2400 4000 4000)
NuMasses=(160 800 1333 267)

avgPileup=40
inputDatasets=('/WRToMuMuJJ_MW-2400_MNu-160_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-e2ebad1b90caaf241f3a6c8cb5bb0f15/USER'
               '/WRToMuMuJJ_MW-2400_MNu-800_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-e2ba59231ce4b9003c4220129f010e56/USER'
               '/WRToMuMuJJ_MW-4000_MNu-1333_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-60196d25a929b65822cbc558e3b0efe7/USER'
               '/WRToMuMuJJ_MW-4000_MNu-267_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-d5aafc3bc7642aee780d5739180b0832/USER')

for h in ${!inputDatasets[*]}
do
	echo 'crab submit -c WR_MWr${wrMasses[$h]}_to_ENu_MNu${nuMasses[$h]}_13TeV_DIGIL1DIGI2RAW_RAW2DIGIL1RecoRECO_${avgPileup}PU_crab.py'
	#eval 'crab submit -c WR_MWr${wrMasses[$h]}_to_ENu_MNu${nuMasses[$h]}_13TeV_DIGIL1DIGI2RAW_RAW2DIGIL1RecoRECO_${avgPileup}PU_crab.py'

done

