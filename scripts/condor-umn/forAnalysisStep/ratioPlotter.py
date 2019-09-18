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
    print "This program takes two histograms (stacks), normalizes them and plots them and their ratio (first over last)"
    print "In order, please specify:"
    print "First root file with stack"
    print "Second root file with a stack (include root structure)"
    print "Directory for output root file:"
    print "Mode (STACK||HIST)"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python ratioPlotter.py /uscms/home/aevans26/nobackup/backgroundStacks//analysis/eventsPassingExtensionRECO2016VETOZMASS/subleadMuonSelMuonPt.root /uscms/home/aevans26/nobackup/dataStacks//analysis/eventsPassingExtensionRECO2016VETOZMASS/subleadMuonSelMuonPt.root ./zStuff STACK"
    print ""
    exit(0)
if len(sys.argv) != 5:
    print len(sys.argv)
    print "inputs not understood, try help/h"
    exit(1)

plotNameFullA = sys.argv[1]
plotNameFullB = sys.argv[2]

output = sys.argv[3]

mode = sys.argv[4]

plotNameA = plotNameFullA.split("/")[-1].split(".")[0]
plotNameB = plotNameFullB.split("/")[-1].split(".")[0]

print plotNameA
print plotNameB

c = ROOT.TCanvas("c","c",1000,1000)

c.Divide(1,2)

if (mode == "HIST"):
    myFile = ROOT.TFile.Open(rootFileOrPath, "read")
    
    histoA = myFile.Get(plotNameFullA)
    histoB = myFile.Get(plotNameFullB)
    
if (mode == "STACK"):
    fileA = ROOT.TFile.Open(plotNameFullA, "read")
    fileB = ROOT.TFile.Open(plotNameFullB, "read")

    histoA = histFromStack(fileA.Get(plotNameA)) 
    histoB = histFromStack(fileB.Get(plotNameB)) 

histoA.Sumw2()
histoB.Sumw2()

nBinsA = histoA.GetNbinsX()
nBinsB = histoB.GetNbinsX()

integralA = 0.0
integralB = 0.0

if (nBinsA != nBinsB):
    print "incompatible histogram binning"
    exit(0)

print plotNameFullA
print plotNameFullB

print "NORMALIZING"
for ibin in range(1,nBinsA+1) :
    print "Bin: "+str(ibin)
    integralA = integralA + histoA.GetBinContent(ibin) 
    integralB = integralB + histoB.GetBinContent(ibin) 
    "ErrDiff:"
    errSqrA = abs (histoA.GetBinContent(ibin) ) ** 0.5
    errA = histoA.GetBinError(ibin)
    if ( errA != errSqrA ): print "Sqrt: "+str(errSqrA)+" vs Err: "+str(errA)
    errSqrB = abs (histoB.GetBinContent(ibin) ) ** 0.5
    errB = histoB.GetBinError(ibin)
    if ( errB != errSqrB ): print "Sqrt: "+str(errSqrB)+" vs Err: "+str(errB)

    print "Integrals: "
    print integralA
    print integralB

print "underflow"
print histoA.GetBinContent(0)
print histoB.GetBinContent(0)
print "entries"
print histoA.GetEntries()
print histoB.GetEntries()
print "sum of weights"
print histoA.GetSumOfWeights()
print histoB.GetSumOfWeights()
print "integrals"
print histoA.Integral()
print histoB.Integral()

if (integralA == 0.0 or integralB == 0.0):
    print "histogram with 0 for integral... cannot compute"
    exit(0)

#NOW TO NORMALIZE
scaleA = 1./integralA
scaleB = 1./integralB

histoA.Scale(scaleA)
histoB.Scale(scaleB)

histoA.SetAxisRange(0.0, 1000.0)
histoB.SetAxisRange(0.0, 1000.0)
#histoArebin = copy.deepcopy(histoA)
#histoBrebin = copy.deepcopy(histoB)
#histoCrebin = copy.deepcopy(histoC)
#
#histoArebin.Rebin(nBinsA)
#histoBrebin.Rebin(nBinsB)
#histoCrebin.Rebin(nBinsC)
#
#print "A,B,C:"
#print str(histoArebin.GetBinContent(1)) + "+-" + str(histoArebin.GetBinError(1))
#print str(histoBrebin.GetBinContent(1)) + "+-" + str(histoBrebin.GetBinError(1))
#print str(histoCrebin.GetBinContent(1)) + "+-" + str(histoCrebin.GetBinError(1))
#
#dScale = ( histoBrebin.GetBinContent(1) * histoCrebin.GetBinContent(1) ) / histoArebin.GetBinContent(1)
#dErr = ( ( histoBrebin.GetBinError(1) * ( histoCrebin.GetBinContent(1) / histoArebin.GetBinContent(1) ) )**2.0 + ( histoCrebin.GetBinError(1) * ( histoBrebin.GetBinContent(1) / histoArebin.GetBinContent(1) ) )**2.0 + ( histoArebin.GetBinError(1) * ( ( histoBrebin.GetBinContent(1) * histoCrebin.GetBinContent(1) ) / ( histoArebin.GetBinContent(1) )**2.0 ) )**2.0 ) ** 0.5
#print "Estimate D :"
#print str(dScale) + "+-" + str(dErr)
#
##NOW TO CALCULATE THE D DISTRIBUTION
ratioHisto = copy.deepcopy(histoA)
ratioHisto.Scale(0.0)
ratioHisto.Sumw2()

#diScale = dScale / histoArebin.GetBinContent(1)
#diScaleErr = ( ( dErr * ( 1 / histoArebin.GetBinContent(1) ) )**2.0 + ( histoArebin.GetBinError(1) * ( dScale / ( histoArebin.GetBinContent(1) )**2.0 ) )**2.0 ) ** 0.5

for ibin in range(1,nBinsA+1):  
    if(histoB.GetBinContent(ibin) == 0):
        newBinVal = -2.0
        newBinErr = -2.0
        continue
    newBinVal = histoA.GetBinContent(ibin) / histoB.GetBinContent(ibin)
    newBinErr = ( ( histoA.GetBinError(ibin) * ( 1 / histoB.GetBinContent(ibin) ) )**2.0 + ( histoA.GetBinContent(ibin) * ( histoB.GetBinError(ibin) / ( histoB.GetBinContent(ibin) )**2.0 ) )**2.0 ) ** 0.5

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
maxR = ratioHisto.GetMaximum()

maxes = [maxA, maxB]
maxes.sort()

newMax = maxes[-1]*1.2


c.cd(1)
histoA.SetMaximum(newMax)
histoA.SetLineColor(ROOT.kBlack)
histoA.Draw("e")

histoB.SetMaximum(newMax)
histoB.SetLineColor(ROOT.kRed)
histoB.Draw("esame")

c.cd(2)
ratioHisto.SetLineColor(ROOT.kBlack)
ratioHisto.Draw("e")


#c.cd(0)
#c.Divide(1, 2)


c.SaveAs(output+"_ratioPlot.png")
ratioHisto.SaveAs(output+"_ratioAlone.root")
ratioHisto.SaveAs(output+"_ratioAlone.png")

