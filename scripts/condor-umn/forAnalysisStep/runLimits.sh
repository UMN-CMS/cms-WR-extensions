#!/bin/bash

declare -A massPointsNR100GeV=( [1000]=100 [1500]=100 [2000]=100 [2500]=100 [3000]=100 [3500]=100 [4000]=100 [4500]=100 [5000]=100 [5500]=100)

declare -A massPointsNR300GeV=( [1000]=300 [1500]=300 [2000]=300 [2500]=300 [3000]=300 [3500]=300 [4000]=300 [4500]=300 [5000]=300 [5500]=300)

declare -A massPointsNR500GeV=( [2000]=500 [2500]=500 [3000]=500 [3500]=500 [4000]=500 [4500]=500 [5000]=500 [5500]=500)

declare -A massPointsNR700GeV=( [2000]=700 [2500]=700 [3000]=700 [3500]=700 [4000]=700 [4500]=700 [5000]=700 [5500]=700)

declare -A massPointsNR900GeV=( [3000]=900 [3500]=900 [4000]=900 [4500]=900 [5000]=900 [5500]=900)

declare -A massPointsNR1100GeV=( [3500]=1100 [4000]=1100 [4500]=1100 [5000]=1100 [5500]=1100)

declare -A massPointsNR1300GeV=( [4500]=1300 [5000]=1300 [5500]=1300)

declare -A massPointsNR1500GeV=( [4500]=1500 [5000]=1500 [5500]=1500)

declare -A massPointsNR1700GeV=( [5500]=1700)

declare -A massPointsNR1900GeV=( [5500]=1900)

declare -A massPoints10th=( [1000]=100 [1500]=150 [2000]=200 [2500]=250 [3000]=300 [3500]=350 [4000]=400 [4500]=450 [5000]=500)

declare -A massPoints5th=( [1000]=200 [1500]=300 [2000]=400 [2500]=500 [3000]=600 [3500]=700 [4000]=800 [4500]=900 [5000]=1000)

WorkspaceDirectory="/uscms_data/d3/mkrohn/WR/JetMassStudy/CMSSW_8_0_26_patch1/src/ExoAnalysis/cmsWRextensions/Output/doFast_variableBinning_2nd/Workspace/"

for i in "${!massPointsNR100GeV[@]}"; do
	j=${massPointsNR100GeV[$i]}
	echo "WR mass $i"
	echo "NR mass $j"
#        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt"  > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
	combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
#	text2workspace.py $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -o $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.root"
	combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
	mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root
#        combine -M HybridNew --testStat=LHC --frequentist ../../../Output/doFast/Workspace/WR_5000_NR_500_CombinedCard.txt -T 500 --expectedFromGrid 0.5

done

for i in "${!massPointsNR300GeV[@]}"; do
        j=${massPointsNR300GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
#        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt"  > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
#       text2workspace.py $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -o $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.root"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR500GeV[@]}"; do
        j=${massPointsNR500GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR700GeV[@]}"; do
        j=${massPointsNR700GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR900GeV[@]}"; do
        j=${massPointsNR900GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR1100GeV[@]}"; do
        j=${massPointsNR1100GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR1300GeV[@]}"; do
        j=${massPointsNR1300GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR1500GeV[@]}"; do
        j=${massPointsNR1500GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR1700GeV[@]}"; do
        j=${massPointsNR1700GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

for i in "${!massPointsNR1900GeV[@]}"; do
        j=${massPointsNR1900GeV[$i]}
        echo "WR mass $i"
        echo "NR mass $j"
        combineCards.py SignalRegion=$WorkspaceDirectory"WR_"$i"_NR_"$j"_SignalRegion.txt" ZPeak=$WorkspaceDirectory"WR_"$i"_NR_"$j"_ZPeak.txt" FSB=$WorkspaceDirectory"WR_"$i"_NR_"$j"_FSB.txt" > $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt"
        combine $WorkspaceDirectory"WR_"$i"_NR_"$j"_CombinedCard.txt" -m 125 -M AsymptoticLimits --run blind > CMS_WR_$i\_NR_$j\_asymptoticCLs.out
        mv higgsCombineTest.AsymptoticLimits.mH125.root Limits/CMS_WR_$i\_NR_$j\_asymptoticCLs.root

done

