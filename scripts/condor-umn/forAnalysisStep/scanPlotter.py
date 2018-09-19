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
    print "Full name of plot to make including ROOT structure"
    print "Directory of where to save plots"
    print "Text file contain list of root files"
    print "Directory for input root files:"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python scanPlotter.py analysis/eventsPassingExtensionRECO/selectedMuonPt /uscms/homes/a/aevans26/nobackup/plots/signalComparisons/ Signal5.txt /uscms_data/d3/mkrohn/WR/JetMassStudy/CMSSW_8_0_26_patch1/src/ExoAnalysis/cmsWRextensions/Output/JetGENMatching/" 
    print ""
    exit(0)
if len(sys.argv) != 5:
    print "inputs not understood, try help/h"
    exit(1)

plotNameFull = sys.argv[1]
output = sys.argv[2]
inputROOTlist = sys.argv[3]
inputROOTdir = sys.argv[4]

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

leg_y = 0.88 - (2+int(len(ROOTs)/2))*0.03
print leg_y
legend = ROOT.TLegend(0.19,leg_y,0.42,0.88)
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
for ROOTfile in ROOTs:
    print "At point: "+ROOTfile.split("/")[-1][:-5]
    newFile = ROOT.TFile.Open(ROOTfile, "read")
    plotsHolder.append(copy.deepcopy(newFile.Get(plotNameFull)))
    plotsHolder[-1].Draw("samee")
    if ( plotsHolder[-1].GetMaximum() > currentMax ):
        currentMax = plotsHolder[-1].GetMaximum()
    plotsHolder[0].SetMaximum(1.1*currentMax)
    plotsHolder[-1].SetLineColor(colors[offset])
    legend.AddEntry(plotsHolder[-1], ROOTfile.split("/")[-1][:-5], "L")
    offset = offset + 1 
    if (offset == len(colors)):
        offset = 0
    
legend.Draw()
c.SaveAs(output+plotName+".png")
    

print "DONE"
sys.stdout.flush()
