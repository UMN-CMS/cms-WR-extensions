#!/bin/bash

#use integer values for masses and pileup
WrMasses=(2400 2400 4000 4000)
NuMasses=(160 800 1333 267)

avgPileup=40
inputDatasets=('/WRToMuMuJJ_MW-2400_MNu-160_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-e2ebad1b90caaf241f3a6c8cb5bb0f15/USER'
               '/WRToMuMuJJ_MW-2400_MNu-800_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-e2ba59231ce4b9003c4220129f010e56/USER'
               '/WRToMuMuJJ_MW-4000_MNu-1333_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-60196d25a929b65822cbc558e3b0efe7/USER'
               '/WRToMuMuJJ_MW-4000_MNu-267_TuneCUETP8M1_13TeV-pythia8/gnegro-RunIIWinter16_80X_GEN-SIM-d5aafc3bc7642aee780d5739180b0832/USER')


#echo "number of input datasets: ${#inputDatasets[*]}"
#echo "length of first dataset name: ${#inputDatasets[0]}"
for h in ${!inputDatasets[*]}
do
	#make crab config file
	eval "sed 's/MWrUndef/MWr${wrMasses[$h]}/g' WR_MWrUndef_to_ENu_MNuUndef_13TeV_DIGIL1DIGI2RAW_RAW2DIGIL1RecoRECO_NNUUMMPU_crab.py > WR_tempOne_crab.py"
	eval "sed 's/MNuUndef/MNu${nuMasses[$h]}/g'  WR_tempOne_crab.py > WR_tempTwo_crab.py"
	eval "sed 's/NNUUMM/$avgPileup/g' WR_tempTwo_crab.py > WR_tempThree_crab.py"
	
	#the delimiter used in the next sed command cannot be a forward slash
	eval "sed 's@datasetFromDBS@${inputDatasets[$h]}@g' WR_tempThree_crab.py > WR_MWr${wrMasses[$h]}_to_ENu_MNu${nuMasses[$h]}_13TeV_DIGIL1DIGI2RAW_RAW2DIGIL1RecoRECO_${avgPileup}PU_crab.py"
	rm WR_tempOne_crab.py WR_tempTwo_crab.py WR_tempThree_crab.py

	#make python config file which governs the DIGI,L1,RAW,L1Reco,and RECO simulation 
	eval "sed 's/MWrUndef/MWr${wrMasses[$h]}/g' WR_MWrUndef_ToENu_MNuUndef_13TeV_DIGI_L1_DIGI2RAW_RAW2DIGI_L1Reco_RECO_NNUUMMPU.py > WR_tempOne.py"
	eval "sed 's/MNuUndef/MNu${nuMasses[$h]}/g'  WR_tempOne.py > WR_tempTwo.py"
	eval "sed 's/NNUUMM/$avgPileup/g'  WR_tempTwo.py > WR_MWr${wrMasses[$h]}_ToENu_MNu${nuMasses[$h]}_13TeV_DIGI_L1_DIGI2RAW_RAW2DIGI_L1Reco_RECO_${avgPileup}PU.py"
	rm WR_tempOne.py WR_tempTwo.py

done

