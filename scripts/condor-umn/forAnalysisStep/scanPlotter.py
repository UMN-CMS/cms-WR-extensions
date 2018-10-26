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
ROOT.kOrange+7 ,          
#ROOT.kSpring ,          
#ROOT.kTeal   ,         
ROOT.kAzure  ,        
ROOT.kViolet ,         
ROOT.kPink       
]
def PAB(histo):
    nBins = histo.GetNbinsX()
    histo.SetBinContent(0, 0.0) ## AT THIS POINT WE DON'T WANT TO DEAL WITH ANY JUNK IN THERE, WE DO WANT TO PRESERVER OVERFLOW THOUGH
    histo.ComputeIntegral()
    integral = 0.0 
    lbi = 0.0
    for ibin in range(1,nBins+2) :
        integral = integral + histo.GetBinContent(ibin) 

    for ibin in range(0,nBins+1) :
        lbi = lbi + histo.GetBinContent(ibin)
        newContent = ( integral - lbi ) / integral 
	InvariantMass = ibin*100. - 100.
	print "ibin: ", InvariantMass
	print "PAB: ", newContent
        histo.SetBinContent(ibin, newContent)
        histo.SetBinError(ibin, 0.0)

    return histo

def PBL(histo):
    nBins = histo.GetNbinsX()
    histo.SetBinContent(0, 0.0) ## AT THIS POINT WE DON'T WANT TO DEAL WITH ANY JUNK IN THERE, WE DO WANT TO PRESERVER OVERFLOW THOUGH
    histo.ComputeIntegral()
    integral = 0.0 
    lbi = 0.0
    for ibin in range(1,nBins+2) :
        integral = integral + histo.GetBinContent(ibin) 

    for ibin in range(nBins,0,-1) :
        lbi = lbi + histo.GetBinContent(ibin)
        newContent = ( integral - lbi ) / integral 
        histo.SetBinContent(ibin, newContent)
        histo.SetBinError(ibin, 0.0)

    return histo
    #for ibin in range(nBins,0,-1) :

#############################################################################################
#WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root
#SingleMuon_WR_M-2400_LNu_M-240.root
print "STARTING"
if len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "h"):
    print "=========="
    print "This program takes a list of histograms and plots them together"
    print "In order, please specify:"
    print "Full name of plot to make including ROOT structure"
    print "Directory of where to save plots"
    print "Text file contain list of root files"
    print "Directory for input root files:"
    print "plot type REG (regular), PAB(PERCENT ABOVE), PBL (PERCENT BELOW)"
    print "True = plot half samples with dotted lines --> input needs to be ordered correctly"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python scanPlotter.py analysis/eventsPassingExtensionRECO/selectedMuonPt /uscms/homes/a/aevans26/nobackup/plots/signalComparisons/ Signal5.txt /uscms_data/d3/mkrohn/WR/JetMassStudy/CMSSW_8_0_26_patch1/src/ExoAnalysis/cmsWRextensions/Output/JetGENMatching/ REG" 
    print ""
    exit(0)
if len(sys.argv) != 7:
    print "inputs not understood, try help/h"
    exit(1)

plotNameFull = sys.argv[1]
output = sys.argv[2]
inputROOTlist = sys.argv[3]
inputROOTdir = sys.argv[4]
style        = sys.argv[5]
lineStyle    = sys.argv[6]

plotName = plotNameFull.split("/")[-1]

print plotName

ROOTs = []
with open(inputROOTlist) as f:
    lines = f.read().splitlines()

for line in lines:
    ROOTs.append(inputROOTdir+line.strip()+".root")

#print ROOTs
myFile = ROOT.TFile.Open(ROOTs[0], "read")

c = ROOT.TCanvas("c","c",1000,1000)

legend = ROOT.TLegend(0.52,0.58,0.88,0.88)
legend.SetFillStyle(0)
legend.SetBorderSize(0)

#firstPlot = myFile.Get(plotNameFull)
#firstPlot.Draw()
#firstPlot.SetLineColor(colors[0])
#
#legend.AddEntry(firstPlot, ROOTs[0].split("/")[-1][:-5], "L")

offset = 0
currentMax = 0.0

plotsHolder = []
i = 1
frame = ROOT.TH1F("frame","",1000,0,1)
frame.SetMaximum(0.7)
frame.Draw()
for ROOTfile in ROOTs:
    print "At point: "+ROOTfile.split("/")[-1][:-5]
    newFile = ROOT.TFile.Open(ROOTfile, "read")

    if (style == "REG"):
        plotsHolder.append(copy.deepcopy(newFile.Get(plotNameFull)))
    elif (style == "PBL"):
        plotsHolder.append(PBL(copy.deepcopy(newFile.Get(plotNameFull))))
    elif (style == "PAB"):
        plotsHolder.append(PAB(copy.deepcopy(newFile.Get(plotNameFull))))

    plotsHolder[-1].SetLineColor(colors[offset])
    plotsHolder[-1].SetLineWidth(3)
    if(lineStyle == "True" and i%2==0):
        plotsHolder[-1].SetLineStyle(2)
    if(style == "REG"):
      plotsHolder[-1].DrawNormalized("hist same")
    else:
      plotsHolder[-1].Draw("hist same")
    if ( plotsHolder[-1].GetMaximum() > currentMax ):
        currentMax = plotsHolder[-1].GetMaximum()
#    plotsHolder[0].GetYaxis().SetRangeUser(0, 0.75)
#    plotsHolder[0].SetMaximum(1.1*currentMax)
    
    legend.AddEntry(plotsHolder[-1], ROOTfile.split("/")[-1][:-5], "L")
    offset = offset + 1 
    i = i + 1
    if (offset == len(colors)):
        offset = 0
    
legend.Draw()
c.SaveAs(output+plotName+".png")
    

print "DONE"
sys.stdout.flush()
