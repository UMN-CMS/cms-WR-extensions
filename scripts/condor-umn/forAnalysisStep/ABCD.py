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

QpartA = 0.84
QpartB = 0.925
QpartC = 0.24

QpartAerr = 0.84
QpartBerr = 0.925
QpartCerr = 0.24

def histFromStack(stack):
    print "Merging N hists: " + str(stack.GetNhists())
    for iHist in range(0, stack.GetNhists()):
        print iHist
        hist = stack.GetHists()[iHist]
        hist.Sumw2()
    newHist = copy.deepcopy(stack.GetHists()[0])
    newHist.Scale(0.0)
    newHist.Merge(stack.GetHists())
    return newHist


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
    print "Whether using stacks in separate files or hists in the same file as input"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python ABCD.py analysis/eventsPassingFlavorSidebandRECOelePt200_noISO/leadAK8JetElectronMass_noISO analysis/eventsPassingFlavorSidebandRECOelePt200_noISO_samesign/leadAK8JetElectronMass_noISO analysis/eventsPassingFlavorSidebandRECOelePt200_samesign/leadAK8JetElectronMass /uscms/homes/a/aevans26/nobackup/plots/signalComparisons/myPlot /uscms_data/d3/mkrohn/WR/JetMassStudy/CMSSW_8_0_26_patch1/src/ExoAnalysis/cmsWRextensions/Output/DR_JetMuon/WR_M-4000_ToLNu_M-400.root HIST" 
    print "python ABCD.py analysis/eventsPassingFlavorSidebandRECOelePt200_noISO/leadAK8JetElectronMass_noISO analysis/eventsPassingFlavorSidebandRECOelePt200_noISO_samesign/leadAK8JetElectronMass_noISO analysis/eventsPassingFlavorSidebandRECOelePt200_samesign/leadAK8JetElectronMass /uscms/homes/a/aevans26/nobackup/plots/signalComparisons/myPlot /uscms_data/d3/mkrohn/WR/JetMassStudy/CMSSW_8_0_26_patch1/src/ExoAnalysis/cmsWRextensions/Output/ABCD_FSB/BackgroundStack/ STACK" 
    print ""
    exit(0)
if len(sys.argv) != 7:
    print len(sys.argv)
    print "inputs not understood, try help/h"
    exit(1)

plotNameFullA = sys.argv[1]
plotNameFullB = sys.argv[2]
plotNameFullC = sys.argv[3]

output = sys.argv[4]
rootFileOrPath = sys.argv[5]

mode = sys.argv[6]

plotNameA = plotNameFullA.split("/")[-1]
plotNameB = plotNameFullB.split("/")[-1]
plotNameC = plotNameFullC.split("/")[-1]

print plotNameA
print plotNameB
print plotNameC

c = ROOT.TCanvas("c","c",1000,1000)

c.Divide(2,2)

if (mode == "HIST"):
    myFile = ROOT.TFile.Open(rootFileOrPath, "read")
    
    histoA = myFile.Get(plotNameFullA)
    histoB = myFile.Get(plotNameFullB)
    histoC = myFile.Get(plotNameFullC)
    
if (mode == "STACK"):
    fileA = ROOT.TFile.Open(rootFileOrPath+"/"+plotNameFullA+".root", "read")
    fileB = ROOT.TFile.Open(rootFileOrPath+"/"+plotNameFullB+".root", "read")
    fileC = ROOT.TFile.Open(rootFileOrPath+"/"+plotNameFullC+".root", "read")

    histoA = histFromStack(fileA.Get(plotNameA)) 
    histoB = histFromStack(fileB.Get(plotNameB)) 
    histoC = histFromStack(fileC.Get(plotNameC)) 

histoA.Sumw2()
histoB.Sumw2()
histoC.Sumw2()

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
    "ErrDiff:"
    errSqrA = abs (histoA.GetBinContent(ibin) ) ** 0.5
    errA = histoA.GetBinError(ibin)
    if ( errA != errSqrA ): print "Sqrt: "+str(errSqrA)+" vs Err: "+str(errA)
    errSqrB = abs (histoB.GetBinContent(ibin) ) ** 0.5
    errB = histoB.GetBinError(ibin)
    if ( errB != errSqrB ): print "Sqrt: "+str(errSqrB)+" vs Err: "+str(errB)
    errSqrC = abs (histoC.GetBinContent(ibin) ) ** 0.5
    errC = histoC.GetBinError(ibin)
    if ( errC != errSqrC ): print "Sqrt: "+str(errSqrC)+" vs Err: "+str(errC)

    print "Integrals: "
    print integralA
    print integralB
    print integralC

print "underflow"
print histoA.GetBinContent(0)
print histoB.GetBinContent(0)
print histoC.GetBinContent(0)
print "entries"
print histoA.GetEntries()
print histoB.GetEntries()
print histoC.GetEntries()
print "sum of weights"
print histoA.GetSumOfWeights()
print histoB.GetSumOfWeights()
print histoC.GetSumOfWeights()
print "integrals"
print histoA.Integral()
print histoB.Integral()
print histoC.Integral()

if (integralA == 0.0 or integralB == 0.0 or integralC == 0.0):
    print "histogram with 0 for integral... cannot compute"
    exit(0)

#NOW TO CALCULATE THE RATIO

histoArebin = copy.deepcopy(histoA)
histoBrebin = copy.deepcopy(histoB)
histoCrebin = copy.deepcopy(histoC)

histoArebin.Rebin(nBinsA)
histoBrebin.Rebin(nBinsB)
histoCrebin.Rebin(nBinsC)

print "A,B,C:"
print str(histoArebin.GetBinContent(1)) + "+-" + str(histoArebin.GetBinError(1))
print str(histoBrebin.GetBinContent(1)) + "+-" + str(histoBrebin.GetBinError(1))
print str(histoCrebin.GetBinContent(1)) + "+-" + str(histoCrebin.GetBinError(1))

dScale = ( histoBrebin.GetBinContent(1) * histoCrebin.GetBinContent(1) ) / histoArebin.GetBinContent(1)
dErr = ( ( histoBrebin.GetBinError(1) * ( histoCrebin.GetBinContent(1) / histoArebin.GetBinContent(1) ) )**2.0 + ( histoCrebin.GetBinError(1) * ( histoBrebin.GetBinContent(1) / histoArebin.GetBinContent(1) ) )**2.0 + ( histoArebin.GetBinError(1) * ( ( histoBrebin.GetBinContent(1) * histoCrebin.GetBinContent(1) ) / ( histoArebin.GetBinContent(1) )**2.0 ) )**2.0 ) ** 0.5
print "Estimate D :"
print str(dScale) + "+-" + str(dErr)

#NOW TO CALCULATE THE D DISTRIBUTION
ratioHisto = copy.deepcopy(histoA)
ratioHisto.Scale(0.0)
ratioHisto.Sumw2()

diScale = dScale / histoArebin.GetBinContent(1)
diScaleErr = ( ( dErr * ( 1 / histoArebin.GetBinContent(1) ) )**2.0 + ( histoArebin.GetBinError(1) * ( dScale / ( histoArebin.GetBinContent(1) )**2.0 ) )**2.0 ) ** 0.5

for ibin in range(1,nBinsA+1):  
    newBinVal = diScale * histoA.GetBinContent(ibin)
    newBinErr = ( (diScale * histoA.GetBinError(ibin)) ** 2 + (diScaleErr * histoA.GetBinContent(ibin)) ** 2 ) ** 0.5 

    ratioHisto.SetBinContent(ibin, newBinVal)
    ratioHisto.SetBinError(ibin, newBinErr)

#THIS IS MATHEMATICALLY DIFFERENT
#ratioHisto = copy.deepcopy(histoB)
#
#ratioHisto.Multiply(histoC)
#ratioHisto.Divide(histoA)


#DRAW TIME
maxA = histoA.GetMaximum()
maxB = histoB.GetMaximum()
maxC = histoC.GetMaximum()
maxR = ratioHisto.GetMaximum()

maxes = [maxA, maxB, maxC, maxR]
maxes.sort()

newMax = maxes[-1]*1.2


c.cd(1)
histoA.SetMaximum(newMax)
histoA.SetLineColor(ROOT.kBlack)
histoA.Draw("e")

c.cd(2)
histoB.SetMaximum(newMax)
histoB.SetLineColor(ROOT.kBlack)
histoB.Draw("e")

c.cd(3)
histoC.SetMaximum(newMax)
histoC.SetLineColor(ROOT.kBlack)
histoC.Draw("e")

c.cd(4)
ratioHisto.SetMaximum(newMax)
ratioHisto.SetLineColor(ROOT.kBlack)
ratioHisto.Draw("e")

#c.cd(0)
#c.Divide(1, 2)


c.SaveAs(output+"_ABCD.png")
ratioHisto.SaveAs(output+"_ABCD_D.root")
ratioHisto.SaveAs(output+"_ABCD_D.png")

#DOUBLE CHECKING
ratioHisto.Rebin(nBinsA)

print "Estimate D integral: NOTE: uncertainty here is unreliable as the ratioHisto does not contain the bin entry weights of the original distributions"
print str(ratioHisto.GetBinContent(1))+"+-"+str(ratioHisto.GetBinError(1))
