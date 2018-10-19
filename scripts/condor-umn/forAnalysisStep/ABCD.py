import ROOT
import sys
import datetime
import subprocess
import os
import copy
import sets
import collections
import math
import tdrstyle




tdrstyle.setTDRStyle()
ROOT.gStyle.SetPadTopMargin(0.10)
ROOT.gStyle.SetPadLeftMargin(0.16)
ROOT.gStyle.SetPadRightMargin(0.10)
ROOT.gStyle.SetPaintTextFormat("1.1f")
ROOT.gStyle.SetOptFit(0000)
ROOT.gROOT.SetBatch()

colors = [
#ROOT.kWhite  ,      
ROOT.kBlack  ,       
#ROOT.kGray   ,   
ROOT.kRed    ,       
ROOT.kGreen  ,          
ROOT.kBlue   ,       
#ROOT.kYellow ,        
#ROOT.kMagenta,        
ROOT.kCyan   ,       
ROOT.kOrange ,          
#ROOT.kSpring ,          
#ROOT.kTeal   ,         
ROOT.kAzure  ,        
ROOT.kViolet ,         
ROOT.kPink       
]
#############################################################################################
#WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root
#SingleMuon_WR_M-2400_LNu_M-240.root

print "STARTING"
if len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "h"):
    print "=========="
    print "This program takes a list of histograms and plots them together"
    print "In order, please specify:"
    print "Full name of plots including ROOT structure: A B C"
    print "Directory of where to save plot"
    print "Directory for input root file:"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python ABCD.py analysis/eventsPassingFlavorSidebandRECOelePt200_noISO/leadAK8JetElectronMass_noISO analysis/eventsPassingFlavorSidebandRECOelePt200_noISO_samesign/leadAK8JetElectronMass_noISO analysis/eventsPassingFlavorSidebandRECOelePt200_samesign/leadAK8JetElectronMass /uscms/homes/a/aevans26/nobackup/plots/signalComparisons/ /uscms_data/d3/mkrohn/WR/JetMassStudy/CMSSW_8_0_26_patch1/src/ExoAnalysis/cmsWRextensions/Output/DR_JetMuon/WR_M-4000_ToLNu_M-400.root" 
    print ""
    exit(0)
if len(sys.argv) != 6:
    print len(sys.argv)
    print "inputs not understood, try help/h"
    exit(1)

plotNameFullA = sys.argv[1]
plotNameFullB = sys.argv[2]
plotNameFullC = sys.argv[3]
output = sys.argv[4]
rootFile = sys.argv[5]

plotNameA = plotNameFullA.split("/")[-1]
plotNameB = plotNameFullB.split("/")[-1]
plotNameC = plotNameFullC.split("/")[-1]

c = ROOT.TCanvas("c","c",1000,1000)

c.Divide(4)

myFile = ROOT.TFile.Open(rootFile, "read")

histoA = myFile.Get(plotNameFullA)
histoB = myFile.Get(plotNameFullB)
histoC = myFile.Get(plotNameFullC)

nBinsA = histoA.GetNbinsX()
nBinsB = histoB.GetNbinsX()
nBinsC = histoC.GetNbinsX()

integralA = 0.0
integralB = 0.0
integralC = 0.0

if (nBinsA != nBinsB or nBinsA != nBinsC):
    print "incompatible histogram binning"
    exit(0)

print plotNameFullA
print plotNameFullB
print plotNameFullC
for ibin in range(1,nBinsA+1) :
    print "Bin: "+str(ibin)
    integralA = integralA + histoA.GetBinContent(ibin) 
    integralB = integralB + histoB.GetBinContent(ibin) 
    integralC = integralC + histoC.GetBinContent(ibin) 
    print "Integrals: "
    print integralA
    print integralB
    print integralC

print "underflow"
print histoA.GetBinContent(0)
print histoB.GetBinContent(0)
print histoC.GetBinContent(0)

if (integralA == 0.0 or integralB == 0.0 or integralC == 0.0):
    print "histogram with 0 for integral... cannot compute"
    exit(0)

#NOW TO CALCULATE THE RATIO

ratioHisto = copy.deepcopy(histoA)
ratioHisto.Scale(0.0)


for ibin in range(1,nBinsA+1):  
    if histoA.GetBinContent(ibin) == 0:
        print "DIVISION BY ZERO NOT POSSIBLE... SKIPPING BIN "+str(ibin)
        continue

    newBinVal = ( histoB.GetBinContent(ibin) * histoC.GetBinContent(ibin) ) / histoA.GetBinContent(ibin)
    newBinErr = ( ( histoB.GetBinError(ibin) * ( histoC.GetBinContent(ibin) / histoA.GetBinContent(ibin) ) )**2.0 + ( histoC.GetBinError(ibin) * ( histoB.GetBinContent(ibin) / histoA.GetBinContent(ibin) ) )**2.0 + ( histoA.GetBinError(ibin) * ( ( histoB.GetBinContent(ibin) * histoC.GetBinContent(ibin) ) / ( histoA.GetBinContent(ibin) )**2.0 ) )**2.0 ) ** 0.5

    ratioHisto.SetBinContent(ibin, newBinVal)
    ratioHisto.SetBinError(ibin, newBinErr)


