import os
import glob
import math    

import ROOT 
#ROOT.gROOT.Macro("rootlogon.C")

import FWCore.ParameterSet.Config as cms

import sys
from DataFormats.FWLite import Events, Handle

from array import array

#function to get integral of all the histograms
def getIntegral(filename,hists):
    cthisto = filename.Get(hists[0])
    print "cthisto"
    cthisto.Print()
    
    nevents = []
    #first = cthisto.Integral()
    #nevents.append(first/2.0)
    nevents.append(10000.)
    nevents.append(cthisto.GetBinContent(2))
    nevents.append(cthisto.GetBinContent(3))
    nevents.append(cthisto.GetBinContent(4)) 
    nevents.append(cthisto.GetBinContent(5)) 
    nevents.append(cthisto.GetBinContent(6)) 
    nevents.append(cthisto.GetBinContent(7)) 
    nevents.append(cthisto.GetBinContent(8)) 
    nevents.append(cthisto.GetBinContent(9)) 

    return nevents

#function to get cutflow normalized to (nevents passed * xsec * lumi)/nevents total    
def getNormCutflow(array,xsec,lumi,ntotalevents):
    normcutflow = []
    for i in range(len(array)):
        newvalue = (array[i]*xsec*lumi)/ntotalevents
        normcutflow.append(newvalue)

    return normcutflow

#function to get cutflow with comparison to previous cut
def getCompareCutflow(array):
    comparecutflow = [array[1]/array[0], array[3]/array[1], array[4]/array[3], array[5]/array[4], array[6]/array[5], array[7]/array[6], array[8]/array[7], array[9]/array[8]]
    
    return comparecutflow

#function to get the the absolute efficiency cutflow
def getCompareAbsoluteEfficiency(array):
    absoluteEfficiency = [array[1]/array[0], array[2]/array[0], array[3]/array[0], array[4]/array[0], array[5]/array[0], array[6]/array[0], array[7]/array[0], array[8]/array[0]]
    
    return absoluteEfficiency

#input files
#WP600 = ROOT.TFile("plots_WP_600_HH.root")
#WP650 = ROOT.TFile("plots_WP_650_HH.root")
#WP700 = ROOT.TFile("plots_WP_700_HH.root")
#WP750 = ROOT.TFile("plots_WP_750_HH.root")
WP5000_500 = ROOT.TFile("../../../Output/CutFlow/WR_M-5000_ToLNu_M-500.root")
WP5000_1000 = ROOT.TFile("../../../Output/CutFlow/WR_M-5000_ToLNu_M-1000.root")
WP5000_1650 = ROOT.TFile("../../../Output/CutFlow/WR_M-5000_ToLNu_M-1650.root")
WP4500_1500 = ROOT.TFile("../../../Output/CutFlow/WR_M-4500_ToLNu_M-1500.root")
WP4500_900 = ROOT.TFile("../../../Output/CutFlow/WR_M-4500_ToLNu_M-900.root")
WP4500_450 = ROOT.TFile("../../../Output/CutFlow/WR_M-4500_ToLNu_M-450.root")
WP4000_1300 = ROOT.TFile("../../../Output/CutFlow/WR_M-4000_ToLNu_M-1300.root")
WP4000_800 = ROOT.TFile("../../../Output/CutFlow/WR_M-4000_ToLNu_M-800.root")
WP4000_400 = ROOT.TFile("../../../Output/CutFlow/WR_M-4000_ToLNu_M-400.root")
WP3500_1150 = ROOT.TFile("../../../Output/CutFlow/WR_M-3500_ToLNu_M-1150.root")
WP3500_700 = ROOT.TFile("../../../Output/CutFlow/WR_M-3500_ToLNu_M-700.root")
WP3500_350 = ROOT.TFile("../../../Output/CutFlow/WR_M-3500_ToLNu_M-350.root")
WP3000_1000 = ROOT.TFile("../../../Output/CutFlow/WR_M-3000_ToLNu_M-1000.root")
WP3000_600 = ROOT.TFile("../../../Output/CutFlow/WR_M-3000_ToLNu_M-600.root")
WP3000_300 = ROOT.TFile("../../../Output/CutFlow/WR_M-3000_ToLNu_M-300.root")
WP2500_800 = ROOT.TFile("../../../Output/CutFlow/WR_M-2500_ToLNu_M-800.root")
WP2500_500 = ROOT.TFile("../../../Output/CutFlow/WR_M-2500_ToLNu_M-500.root")
WP2500_250 = ROOT.TFile("../../../Output/CutFlow/WR_M-2500_ToLNu_M-250.root")
WP2000_650 = ROOT.TFile("../../../Output/CutFlow/WR_M-2000_ToLNu_M-650.root")
WP2000_400 = ROOT.TFile("../../../Output/CutFlow/WR_M-2000_ToLNu_M-400.root")
WP2000_200 = ROOT.TFile("../../../Output/CutFlow/WR_M-2000_ToLNu_M-200.root")
WP1500_500 = ROOT.TFile("../../../Output/CutFlow/WR_M-1500_ToLNu_M-500.root")
WP1500_300 = ROOT.TFile("../../../Output/CutFlow/WR_M-1500_ToLNu_M-300.root")
WP1500_150 = ROOT.TFile("../../../Output/CutFlow/WR_M-1500_ToLNu_M-150.root")
WP1000_300 = ROOT.TFile("../../../Output/CutFlow/WR_M-1000_ToLNu_M-300.root")
WP1000_200 = ROOT.TFile("../../../Output/CutFlow/WR_M-1000_ToLNu_M-200.root")
WP1000_100 = ROOT.TFile("../../../Output/CutFlow/WR_M-1000_ToLNu_M-100.root")


#ttbar = ROOT.TFile("plots_TT.root")
#QCD300 = ROOT.TFile("plots_QCD300.root")
#QCD500 = ROOT.TFile("plots_QCD500.root")
#QCD700 = ROOT.TFile("plots_QCD700.root")
#QCD1000 = ROOT.TFile("plots_QCD1000.root")
#QCD1500 = ROOT.TFile("plots_QCD1500.root")
#QCD2000 = ROOT.TFile("plots_QCD2000.root")

#sample names
#sample1name = "HH 600"
#sample2name = "HH 650"
#sample3name = "HH 700"
#sample4name = "HH 750"
sample1name = "$W_{R}$ 5000 GeV, $N_{R}$ 1650 GeV"
sample2name = "$W_{R}$ 5000 GeV, $N_{R}$ 1000 GeV"
sample3name = "$W_{R}$ 5000 GeV, $N_{R}$ 500 GeV"
sample4name = "$W_{R}$ 4500 GeV, $N_{R}$ 1500 GeV"
sample5name = "$W_{R}$ 4500 GeV, $N_{R}$ 900 GeV"
sample6name = "$W_{R}$ 4500 GeV, $N_{R}$ 450 GeV"
sample7name = "$W_{R}$ 4000 GeV, $N_{R}$ 1300 GeV"
sample8name = "$W_{R}$ 4000 GeV, $N_{R}$ 800 GeV"
sample9name = "$W_{R}$ 4000 GeV, $N_{R}$ 400 GeV"
sample10name = "$W_{R}$ 3500 GeV, $N_{R}$ 1150 GeV"
sample11name = "$W_{R}$ 3500 GeV, $N_{R}$ 700 GeV"
sample12name = "$W_{R}$ 3500 GeV, $N_{R}$ 350 GeV"
sample13name = "$W_{R}$ 3000 GeV, $N_{R}$ 1000 GeV"
sample14name = "$W_{R}$ 3000 GeV, $N_{R}$ 600 GeV"
sample15name = "$W_{R}$ 3000 GeV, $N_{R}$ 300 GeV"
sample16name = "$W_{R}$ 2500 GeV, $N_{R}$ 800 GeV"
sample17name = "$W_{R}$ 2500 GeV, $N_{R}$ 500 GeV"
sample18name = "$W_{R}$ 2500 GeV, $N_{R}$ 250 GeV"
sample19name = "$W_{R}$ 2000 GeV, $N_{R}$ 650 GeV"
sample20name = "$W_{R}$ 2000 GeV, $N_{R}$ 400 GeV"
sample21name = "$W_{R}$ 2000 GeV, $N_{R}$ 200 GeV"
sample22name = "$W_{R}$ 1500 GeV, $N_{R}$ 500 GeV"
sample23name = "$W_{R}$ 1500 GeV, $N_{R}$ 300 GeV"
sample24name = "$W_{R}$ 1500 GeV, $N_{R}$ 150 GeV"
sample25name = "$W_{R}$ 1000 GeV, $N_{R}$ 300 GeV"
sample26name = "$W_{R}$ 1000 GeV, $N_{R}$ 200 GeV"
sample27name = "$W_{R}$ 1000 GeV, $N_{R}$ 100 GeV"

#histograms for cutflow
#histograms = ["bbj","bb0","bb1","bb2","bb3","bb4","bb5","bb6","bb7","bb8","bb9","bb10"]
histograms = ["analysis/allEvents/cutProgress"]

#initializing cutflow arrays
myn1 = []
myn2 = []
myn3 = []
myn4 = []
myn5 = []
myn6 = []
myn7 = []
myn8 = []
myn9 = []
myn10 = []
myn11 = []
myn12 = []
myn13 = []
myn14 = []
myn15 = []
myn16 = []
myn17 = []
myn18 = []
myn19 = []
myn20 = []
myn21 = []
myn22 = []
myn23 = []
myn24 = []
myn25 = []
myn26 = []
myn27 = []


#writing cutflow arrays
#myn1 = getIntegral(WP5000_1650,histograms)
myn2 = getIntegral(WP5000_1000,histograms)
myn3 = getIntegral(WP5000_500,histograms)
myn4 = getIntegral(WP4500_1500,histograms)
myn5 = getIntegral(WP4500_900,histograms)
myn6 = getIntegral(WP4500_450,histograms)
myn7 = getIntegral(WP4000_1300,histograms)
#myn8 = getIntegral(WP4000_800,histograms)
#myn9 = getIntegral(WP4000_400,histograms)
#myn10 = getIntegral(WP3500_1150,histograms)
myn11 = getIntegral(WP3500_700,histograms)
#myn12 = getIntegral(WP3500_350,histograms)
myn13 = getIntegral(WP3000_1000,histograms)
myn14 = getIntegral(WP3000_600,histograms)
myn15 = getIntegral(WP3000_300,histograms)
#myn16 = getIntegral(WP2500_800,histograms)
#myn17 = getIntegral(WP2500_500,histograms)
#myn18 = getIntegral(WP2500_250,histograms)
myn19 = getIntegral(WP2000_650,histograms)
#myn20 = getIntegral(WP2000_400,histograms)
#myn21 = getIntegral(WP2000_200,histograms)
#myn22 = getIntegral(WP1500_500,histograms)
#myn23 = getIntegral(WP1500_300,histograms)
#myn24 = getIntegral(WP1500_150,histograms)
#myn25 = getIntegral(WP1000_300,histograms)
#myn26 = getIntegral(WP1000_200,histograms)
#myn27 = getIntegral(WP1000_100,histograms)

#writing normalized cutflow arrays
#mync1 = getNormCutflow(myn1, 1, 9200, 98000)
#mync2 = getNormCutflow(myn2, 1, 9200, 100000)
#mync3 = getNormCutflow(myn3, 1, 9200, 100000)
#mync4 = getNormCutflow(myn4, 1, 9200, 95600)
#mync13 = getNormCutflow(myn5, 1, 9200, 95800)
#mync14 = getNormCutflow(myn6, 1, 9200, 100000)
#mync15 = getNormCutflow(myn7, 1, 9200, 50000)
#mync16 = getNormCutflow(myn8, 1, 9200, 50000)
#mync17 = getNormCutflow(myn9, 1, 9200, 50000)
#mync18 = getNormCutflow(myn10, 1, 9200, 50000)
#mync19 = getNormCutflow(myn11, 1, 9200, 50000)
#mync20 = getNormCutflow(myn12, 1, 9200, 50000)
#mync21 = getNormCutflow(myn13, 1, 9200, 48800)
#mync22 = getNormCutflow(myn14, 1, 9200, 23600)
#mync23 = getNormCutflow(myn15, 1, 9200, 50000)
#mync24 = getNormCutflow(myn16, 1, 9200, 49600)
#mync1 = getNormCutflow(myn16, 1, 9200, 100000)
#mync2 = getNormCutflow(myn16, 1, 9200, 100000)
#mync3 = getNormCutflow(myn16, 1, 9200, 50000)
#mync4 = getNormCutflow(myn16, 1, 9200, 49000)
#mync5 = getNormCutflow(myn16, 1, 9200, 48000)
#mync6 = getNormCutflow(myn16, 1, 9200, 50000)
#mync7 = getNormCutflow(myn16, 1, 9200, 48000)
#mync8 = getNormCutflow(myn16, 1, 9200, 50000)
#mync9 = getNormCutflow(myn16, 1, 9200, 48200)
#mync10 = getNormCutflow(myn16, 1, 9200, 50000)
#mync11 = getNormCutflow(myn16, 1, 9200, 50000)
#mync12 = getNormCutflow(myn16, 1, 9200, 50000)

#mync17 = getNormCutflow(myn17, 831.76, 9200, 32106228.)
#mync18 = getNormCutflow(myn18, 347700., 9200, 16763036.)
#mync19 = getNormCutflow(myn19, 32100., 9200, 19198108.)
#mync20 = getNormCutflow(myn20, 6831., 9200, 15616919.)
#mync21 = getNormCutflow(myn21, 1207., 9200, 4983341.)
#mync22 = getNormCutflow(myn22, 119.9, 9200, 3753851.)
#mync23 = getNormCutflow(myn23, 25.24, 9200, 1959483.)

#get comparative cutflows
#mycc1 = getCompareCutflow(myn1)
#mycc2 = getCompareCutflow(myn2)
#mycc3 = getCompareCutflow(myn3)
#mycc4 = getCompareCutflow(myn4)
#mycc5 = getCompareCutflow(myn5)
#mycc6 = getCompareCutflow(myn6)
#mycc7 = getCompareCutflow(myn7)
#mycc8 = getCompareCutflow(myn8)
#mycc9 = getCompareCutflow(myn9)
#mycc10 = getCompareCutflow(myn10)
#mycc11 = getCompareCutflow(myn11)
#mycc12 = getCompareCutflow(myn12)
#mycc13 = getCompareCutflow(myn13)
#mycc14 = getCompareCutflow(myn14)
#mycc15 = getCompareCutflow(myn15)
#mycc16 = getCompareCutflow(myn16)
#mycc17 = getCompareCutflow(myn17)
#mycc18 = getCompareCutflow(myn18)
#mycc19 = getCompareCutflow(myn19)
#mycc20 = getCompareCutflow(myn20)
#mycc21 = getCompareCutflow(myn21)
#mycc22 = getCompareCutflow(myn22)
#mycc23 = getCompareCutflow(myn23)
#mycc24 = getCompareCutflow(myn24)

#myAE1 = getCompareAbsoluteEfficiency(myn1)
myAE2 = getCompareAbsoluteEfficiency(myn2)
myAE3 = getCompareAbsoluteEfficiency(myn3)
myAE4 = getCompareAbsoluteEfficiency(myn4)
myAE5 = getCompareAbsoluteEfficiency(myn5)
myAE6 = getCompareAbsoluteEfficiency(myn6)
myAE7 = getCompareAbsoluteEfficiency(myn7)
#myAE8 = getCompareAbsoluteEfficiency(myn8)
#myAE9 = getCompareAbsoluteEfficiency(myn9)
#myAE10 = getCompareAbsoluteEfficiency(myn10)
myAE11 = getCompareAbsoluteEfficiency(myn11)
#myAE12 = getCompareAbsoluteEfficiency(myn12)
myAE13 = getCompareAbsoluteEfficiency(myn13)
myAE14 = getCompareAbsoluteEfficiency(myn14)
myAE15 = getCompareAbsoluteEfficiency(myn15)
#myAE16 = getCompareAbsoluteEfficiency(myn16)
#myAE17 = getCompareAbsoluteEfficiency(myn17)
#myAE18 = getCompareAbsoluteEfficiency(myn18)
myAE19 = getCompareAbsoluteEfficiency(myn19)
#myAE20 = getCompareAbsoluteEfficiency(myn20)
#myAE21 = getCompareAbsoluteEfficiency(myn21)
#myAE22 = getCompareAbsoluteEfficiency(myn22)
#myAE23 = getCompareAbsoluteEfficiency(myn23)
#myAE24 = getCompareAbsoluteEfficiency(myn24)
#myAE25 = getCompareAbsoluteEfficiency(myn25)
#myAE26 = getCompareAbsoluteEfficiency(myn26)
#myAE27 = getCompareAbsoluteEfficiency(myn27)

#writing tex file
target = open("CutFlowTable_WR_Signals.tex",'w')
#target = open("cutflowtable_76X_vh_ZP_DDT.tex",'w')
 
target.write("\documentclass{article}")
target.write("\n")
target.write("\usepackage[a4paper,margin=1in,landscape]{geometry}")
target.write("\n")
target.write("\oddsidemargin=0in")
target.write("\n")
target.write("\evensidemargin=0in")
target.write("\n")
target.write("\n")
target.write("\\begin{document}")
target.write("\n")
target.write("\n")
#third table, comparing the absolute efficiency
target.write("\n")
target.write("\\begin{table}[h]")
target.write("\n")
target.write("\small")
target.write("\n")
target.write("\\begin{tabular}{|l|c|c|c|c|c|c|c|c|}")
target.write("\n")
target.write("\hline")
target.write("\n")
target.write("Sample & $> 0$ AK8 Jet & $> 0$ High $p_{T}$ Mu & $> 0$ Mu + Jet Pair & $> 0$ Loose ID Mu & DiMu Mass $> 200$ GeV & Trigger \\\ \hline")
#target.write("Sample & $> 0$ AK8 Jet & $> 0$ High $p_{T}$ Mu & $> 0$ Mu + Jet Pair & $> 0$ Loose ID Mu & DiMu Mass $> 200$ GeV & Trigger & 1 High $p_{T}$ Mu & 1 Loose ID Mu \\\ \hline")
target.write("\n")
#target.write(sample1name + " & " + str(round(myAE1[0],3))+ " & " + str(round(myAE1[1],3)) + " & " + str(round(myAE1[2],3)) + " & " + str(round(myAE1[3],3)) + " & " + str(round(myAE1[4],3)) + " & " + str(round(myAE1[5],3)) + " & " + str(round(myAE1[6],3)) + " & " + str(round(myAE1[7],3)) + " \\\ " )
#target.write("\n")
target.write(sample2name + " & " + str(round(myAE2[0],3))+ " & " + str(round(myAE2[1],3)) + " & " + str(round(myAE2[2],3)) + " & " + str(round(myAE2[3],3)) + " & " + str(round(myAE2[4],3)) + " & " + str(round(myAE2[5],3)) + " & " + " \\\ " )
target.write("\n")
target.write(sample3name + " & " + str(round(myAE3[0],3))+ " & " + str(round(myAE3[1],3)) + " & " + str(round(myAE3[2],3)) + " & " + str(round(myAE3[3],3)) + " & " + str(round(myAE3[4],3)) + " & " + str(round(myAE3[5],3)) + " & " + " \\\ " )
target.write("\n")
target.write(sample4name + " & " + str(round(myAE4[0],3))+ " & " + str(round(myAE4[1],3)) + " & " + str(round(myAE4[2],3)) + " & " + str(round(myAE4[3],3)) + " & " + str(round(myAE4[4],3)) + " & " + str(round(myAE4[5],3)) + " & " + " \\\ " )
target.write("\n")
target.write(sample5name + " & " + str(round(myAE5[0],3))+ " & " + str(round(myAE5[1],3)) + " & " + str(round(myAE5[2],3)) + " & " + str(round(myAE5[3],3)) + " & " + str(round(myAE5[4],3)) + " & " + str(round(myAE5[5],3)) + " & " + " \\\ " )
target.write("\n")
target.write(sample6name + " & " + str(round(myAE6[0],3))+ " & " + str(round(myAE6[1],3)) + " & " + str(round(myAE6[2],3)) + " & " + str(round(myAE6[3],3)) + " & " + str(round(myAE6[4],3)) + " & " + str(round(myAE6[5],3)) + " & " + " \\\ " )
target.write("\n")
target.write(sample7name + " & " + str(round(myAE7[0],3))+ " & " + str(round(myAE7[1],3)) + " & " + str(round(myAE7[2],3)) + " & " + str(round(myAE7[3],3)) + " & " + str(round(myAE7[4],3)) + " & " + str(round(myAE7[5],3)) + " & " + " \\\ " )
target.write("\n")
#target.write(sample8name + " & " + str(round(myAE8[0],3))+ " & " + str(round(myAE8[1],3)) + " & " + str(round(myAE8[2],3)) + " & " + str(round(myAE8[3],3)) + " & " + str(round(myAE8[4],3)) + " & " + str(round(myAE8[5],3)) + " & " + str(round(myAE8[6],3)) + " & " + str(round(myAE8[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample9name + " & " + str(round(myAE9[0],3))+ " & " + str(round(myAE9[1],3)) + " & " + str(round(myAE9[2],3)) + " & " + str(round(myAE9[3],3)) + " & " + str(round(myAE9[4],3)) + " & " + str(round(myAE9[5],3)) + " & " + str(round(myAE9[6],3)) + " & " + str(round(myAE9[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample10name + " & " + str(round(myAE10[0],3))+ " & " + str(round(myAE10[1],3)) + " & " + str(round(myAE10[2],3)) + " & " + str(round(myAE10[3],3)) + " & " + str(round(myAE10[4],3)) + " & " + str(round(myAE10[5],3)) + " & " + str(round(myAE10[6],3)) + " & " + str(round(myAE10[7],3)) + " \\\ " )
#target.write("\n")
target.write(sample11name + " & " + str(round(myAE11[0],3))+ " & " + str(round(myAE11[1],3)) + " & " + str(round(myAE11[2],3)) + " & " + str(round(myAE11[3],3)) + " & " + str(round(myAE11[4],3)) + " & " + str(round(myAE11[5],3)) + " & " + " \\\ " )
target.write("\n")
#target.write(sample12name + " & " + str(round(myAE12[0],3))+ " & " + str(round(myAE12[1],3)) + " & " + str(round(myAE12[2],3)) + " & " + str(round(myAE12[3],3)) + " & " + str(round(myAE12[4],3)) + " & " + str(round(myAE12[5],3)) + " & " + str(round(myAE12[6],3)) + " & " + str(round(myAE12[7],3)) + " \\\ " )
#target.write("\n")
target.write(sample13name + " & " + str(round(myAE13[0],3))+ " & " + str(round(myAE13[1],3)) + " & " + str(round(myAE13[2],3)) + " & " + str(round(myAE13[3],3)) + " & " + str(round(myAE13[4],3)) + " & " + str(round(myAE13[5],3)) + " & " + " \\\ " )
target.write("\n")
target.write(sample14name + " & " + str(round(myAE14[0],3))+ " & " + str(round(myAE14[1],3)) + " & " + str(round(myAE14[2],3)) + " & " + str(round(myAE14[3],3)) + " & " + str(round(myAE14[4],3)) + " & " + str(round(myAE14[5],3)) + " & " + " \\\ " )
target.write("\n")
target.write(sample15name + " & " + str(round(myAE15[0],3))+ " & " + str(round(myAE15[1],3)) + " & " + str(round(myAE15[2],3)) + " & " + str(round(myAE15[3],3)) + " & " + str(round(myAE15[4],3)) + " & " + str(round(myAE15[5],3)) + " & " + " \\\ " )
target.write("\n")
#target.write(sample16name + " & " + str(round(myAE16[0],3))+ " & " + str(round(myAE16[1],3)) + " & " + str(round(myAE16[2],3)) + " & " + str(round(myAE16[3],3)) + " & " + str(round(myAE16[4],3)) + " & " + str(round(myAE16[5],3)) + " & " + str(round(myAE16[6],3)) + " & " + str(round(myAE16[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample17name + " & " + str(round(myAE17[0],3))+ " & " + str(round(myAE17[1],3)) + " & " + str(round(myAE17[2],3)) + " & " + str(round(myAE17[3],3)) + " & " + str(round(myAE17[4],3)) + " & " + str(round(myAE17[5],3)) + " & " + str(round(myAE17[6],3)) + " & " + str(round(myAE17[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample18name + " & " + str(round(myAE18[0],3))+ " & " + str(round(myAE18[1],3)) + " & " + str(round(myAE18[2],3)) + " & " + str(round(myAE18[3],3)) + " & " + str(round(myAE18[4],3)) + " & " + str(round(myAE18[5],3)) + " & " + str(round(myAE18[6],3)) + " & " + str(round(myAE18[7],3)) + " \\\ " )
#target.write("\n")
target.write(sample19name + " & " + str(round(myAE19[0],3))+ " & " + str(round(myAE19[1],3)) + " & " + str(round(myAE19[2],3)) + " & " + str(round(myAE19[3],3)) + " & " + str(round(myAE19[4],3)) + " & " + str(round(myAE19[5],3)) + " & " + " \\\ " )
target.write("\n")
#target.write(sample20name + " & " + str(round(myAE20[0],3))+ " & " + str(round(myAE20[1],3)) + " & " + str(round(myAE20[2],3)) + " & " + str(round(myAE20[3],3)) + " & " + str(round(myAE20[4],3)) + " & " + str(round(myAE20[5],3)) + " & " + str(round(myAE20[6],3)) + " & " + str(round(myAE20[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample21name + " & " + str(round(myAE21[0],3))+ " & " + str(round(myAE21[1],3)) + " & " + str(round(myAE21[2],3)) + " & " + str(round(myAE21[3],3)) + " & " + str(round(myAE21[4],3)) + " & " + str(round(myAE21[5],3)) + " & " + str(round(myAE21[6],3)) + " & " + str(round(myAE21[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample22name + " & " + str(round(myAE22[0],3))+ " & " + str(round(myAE22[1],3)) + " & " + str(round(myAE22[2],3)) + " & " + str(round(myAE22[3],3)) + " & " + str(round(myAE22[4],3)) + " & " + str(round(myAE22[5],3)) + " & " + str(round(myAE22[6],3)) + " & " + str(round(myAE22[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample23name + " & " + str(round(myAE23[0],3))+ " & " + str(round(myAE23[1],3)) + " & " + str(round(myAE23[2],3)) + " & " + str(round(myAE23[3],3)) + " & " + str(round(myAE23[4],3)) + " & " + str(round(myAE23[5],3)) + " & " + str(round(myAE23[6],3)) + " & " + str(round(myAE23[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample24name + " & " + str(round(myAE24[0],3))+ " & " + str(round(myAE24[1],3)) + " & " + str(round(myAE24[2],3)) + " & " + str(round(myAE24[3],3)) + " & " + str(round(myAE24[4],3)) + " & " + str(round(myAE24[5],3)) + " & " + str(round(myAE24[6],3)) + " & " + str(round(myAE24[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample25name + " & " + str(round(myAE25[0],3))+ " & " + str(round(myAE25[1],3)) + " & " + str(round(myAE25[2],3)) + " & " + str(round(myAE25[3],3)) + " & " + str(round(myAE25[4],3)) + " & " + str(round(myAE25[5],3)) + " & " + str(round(myAE25[6],3)) + " & " + str(round(myAE25[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample26name + " & " + str(round(myAE26[0],3))+ " & " + str(round(myAE26[1],3)) + " & " + str(round(myAE26[2],3)) + " & " + str(round(myAE26[3],3)) + " & " + str(round(myAE26[4],3)) + " & " + str(round(myAE26[5],3)) + " & " + str(round(myAE26[6],3)) + " & " + str(round(myAE26[7],3)) + " \\\ " )
#target.write("\n")
#target.write(sample27name + " & " + str(round(myAE27[0],3))+ " & " + str(round(myAE27[1],3)) + " & " + str(round(myAE27[2],3)) + " & " + str(round(myAE27[3],3)) + " & " + str(round(myAE27[4],3)) + " & " + str(round(myAE27[5],3)) + " & " + str(round(myAE27[6],3)) + " & " + str(round(myAE27[7],3)) + " \\\ " )
#target.write("\n")
target.write("\hline")
target.write("\n")
target.write("\end{tabular}")
#target.write("\n")
#target.write("\caption{ Absolute Efficiency in Signal Region }")
#target.write("\n")
target.write("\end{table}")
target.write("\end{document}")



target.close()
