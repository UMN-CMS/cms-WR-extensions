#!/bin/bash

declare -A massPoints10th=( [1000]=100 [1500]=150 [2000]=200 [2500]=250 [3000]=300 [3500]=350 [4000]=400 [4500]=450 [5000]=500)

declare -A massPoints5th=( [1000]=200 [1500]=300 [2000]=400 [2500]=500 [3000]=600 [3500]=700 [4000]=800 [4500]=900 [5000]=1000)

WorkspaceDirectory="/uscms_data/d3/mkrohn/WR/JetMassStudy/CMSSW_8_0_26_patch1/src/ExoAnalysis/cmsWRextensions/Output/doFast/Workspace/"

for i in "${!massPoints10th[@]}"; do
	j=${massPoints10th[$i]}
	echo "WR mass $i"
	echo "NR mass $j"
#        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt"  > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
	combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
#	text2workspace.py $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -o $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.root"
	combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
	mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root
#        combine -M HybridNew --testStat=LHC --frequentist ../../../Output/doFast/Workspace/WR_5000_NR_500_CombinedCard.txt -T 500 --expectedFromGrid 0.5

done

for i in "${!massPoints5th[@]}"; do
        j=${massPoints5th[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
#        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt"  > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
#       text2workspace.py $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -o $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.root"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

