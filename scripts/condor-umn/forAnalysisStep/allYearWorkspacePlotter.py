import ROOT
import sys
import datetime
import subprocess
import os
import copy
import sets
import collections
import math
from array import array
from numpy import log



#INITIAL SETUP STUFF

#STYLE#########################
ROOT.gStyle.SetPadTopMargin(0.10)
ROOT.gStyle.SetPadLeftMargin(0.16)
ROOT.gStyle.SetPadRightMargin(0.10)
ROOT.gStyle.SetPaintTextFormat("1.1f")
ROOT.gStyle.SetOptFit(0000)
ROOT.gStyle.SetOptStat(0)
ROOT.gROOT.SetBatch()

#BACKGROUNDS LIST AND COLORS####################
##BOTTOM TO TOP##
bgsRes = collections.OrderedDict()

bgsRes["WJets"] = 870
bgsRes["ttV"] = (ROOT.kOrange+2)
bgsRes["TriBoson"] = (ROOT.kMagenta)
bgsRes["DiBoson"] = (ROOT.kSpring-1)
bgsRes["ST"] = (ROOT.kRed+2)
bgsRes["DY"] = ROOT.kYellow
bgsRes["EMu"] = ROOT.kRed

bgsBoost = collections.OrderedDict()

bgsBoost["ttV"] = (ROOT.kOrange+2)
bgsBoost["TriBoson"] = (ROOT.kMagenta)
bgsBoost["DiBoson"] = (ROOT.kSpring-1)
bgsBoost["WJets"] = 870
bgsBoost["ST"] = (ROOT.kRed+2)
bgsBoost["DY"] = ROOT.kYellow
bgsBoost["EMu"] = ROOT.kRed

#BINNING##############################
binEdgeListBoosted = [800., 1000., 1200., 1400., 1700., 8000.]
binEdgeListResolved= [800., 1000., 1200., 1400., 1600., 2000., 2400., 2800., 3200., 8000.]

binningRes  = array("d",binEdgeListResolved)
binningBoost= array("d",binEdgeListBoosted)

#LUMI###################################
lumi2016 = 35.92
lumi2017 = 41.53
lumi2018 = 59.74

#MISC####################################
folder = "MuMuJJ/"
extension = ".pdf"

#STACK PARAMS################################
hTitle = ""
xTitleRes   = "M_{lljj} (GeV)"
xTitleBoost = "M_{lJ} (GeV)"
yTitle = "Events / bin"
stackOptions = "hist"
xMin = 800
xMax = 8000
yMin = 0.1
yMaxRes   = 5000
yMaxBoost = 5000
#LEGEND###############################
x1 = 0.6
x2 = 0.9
y1 = 0.6
y2 = 0.9

#STYLE#################################
height = 0.92
lumix1 = 0.655
lumix2 = height

cmsx1 = 0.17
cmsx2 = height

prelx1 = 0.27
prelx2 = height

regionx1 = 0.4
regionx2 = 0.85

xtitlesize = 0.06
xlabelsize = 0.05
xoffset    = 0.9

ytitlesize = 0.06
ylabelsize = 0.05
yoffset    = 0.9

botMarg = 0.2
#############################################################################################
if len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "h"):
    print "=========="
    print "This program makes stack plots from the workspaces"
    print "In order, please specify:"
    print "Boosted/Resolved"
    print "Workspace Files 2016, 2017, 2018"
    print "Output Tag"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python workspacePlotter.py Boosted myPath2016/myWorkspace.root myPath2017/myWorkspace.root myPath2018/myWorkspace.root Potatoe"
    print ""
    exit(0)
if len(sys.argv) != 6:
    print "("+str(len(sys.argv))+") inputs not understood, try help/h"
    exit(1)

#READ IN OPTIONS
flavor = sys.argv[1]
inputFile1 = sys.argv[2]
inputFile2 = sys.argv[3]
inputFile3 = sys.argv[4]
tag = sys.argv[5]

##############################################################################################
print "STARTING"

if flavor.upper() == "BOOSTED":
  print "BOOSTED"
  xTitle = xTitleBoost
  yMax = yMaxBoost
  region = "\mu\mu boosted"
  bgs = bgsBoost
elif flavor.upper() == "RESOLVED":
  print "RESOLVED"
  xTitle = xTitleRes
  yMax = yMaxRes
  region = "\mu\mu resolved"
  bgs = bgsRes
else:
  print "NOT A VALID SIGNAL REGION: "+str(flavor)
  exit(1)

lumi = lumi2016 + lumi2017 + lumi2018

c = ROOT.TCanvas("c","c",2000,1500)
legend = ROOT.TLegend(x1,y1,x2,y2)

bgStack = ROOT.THStack()

inFile1 = ROOT.TFile.Open(inputFile1, "read")
inFile2 = ROOT.TFile.Open(inputFile2, "read")
inFile3 = ROOT.TFile.Open(inputFile3, "read")

#BG STACK BUILD#########################
for bg,color in bgs.items(): 
  print "Adding: "+bg

  #MERGE MULTIBOSON
  if(bg == "TriBoson"):
    continue
  bgHist1 = inFile1.Get(folder+bg)
  bgHist2 = inFile2.Get(folder+bg)
  bgHist3 = inFile3.Get(folder+bg)

  bgHist = copy.deepcopy(bgHist1)
  bgHist.Add(bgHist2)
  bgHist.Add(bgHist3)
  if(bg == "DiBoson"):
    bgHist.Add(inFile1.Get(folder+"TriBoson"))
    bgHist.Add(inFile2.Get(folder+"TriBoson"))
    bgHist.Add(inFile3.Get(folder+"TriBoson"))

  bgHist.SetFillColor(color)
  bgHist.SetLineColor(color)
  bgHist.SetMarkerColor(color)

  bgStack.Add(bgHist)
  if(bg == "EMu"):
    legend.AddEntry(bgHist, "ttbar")
  elif(bg == "DiBoson"):
    legend.AddEntry(bgHist, "Multiboson")
  else:
    legend.AddEntry(bgHist, bg)



#DRAW TIME##################################
ROOT.gPad.SetLogy()
ROOT.gPad.SetBorderSize(0)
ROOT.gPad.SetBottomMargin(botMarg)
bgStack.Draw(stackOptions)
legend.Draw()

#MAKE UNCERTAINTY#############################
allMC=bgStack.GetStack().Last().Clone()
herr = allMC.Clone('herr')

theErrorGraph = ROOT.TGraphErrors(herr)
theErrorGraph.SetFillColor(ROOT.kGray+2)
theErrorGraph.SetFillStyle(3003)
herr.SetFillColor(ROOT.kGray+2)
herr.SetFillStyle(3003)
herr.SetMarkerColor(1111);

theErrorGraph.Draw('SAME2')

legend.AddEntry(herr,"MC uncert. (stat.)","fl")

#MAKE PRETTY STACK########################
bgStack.GetXaxis().SetTitle(xTitle)
bgStack.GetXaxis().SetRangeUser(xMin,xMax)
bgStack.SetTitle(hTitle)
bgStack.GetYaxis().SetTitle(yTitle)
bgStack.SetMinimum(yMin)
bgStack.SetMaximum(yMax)
bgStack.GetXaxis().SetLabelSize(xlabelsize)
bgStack.GetXaxis().SetTitleSize(xtitlesize)
bgStack.GetXaxis().SetTitleOffset(xoffset)
bgStack.GetYaxis().SetLabelSize(ylabelsize)
bgStack.GetYaxis().SetTitleSize(ytitlesize)
bgStack.GetYaxis().SetTitleOffset(yoffset)
#STYLE STUFF##############################
tag1 = ROOT.TLatex(lumix1,lumix2,"%.1f fb^{-1} (13 TeV)"%lumi)
tag1.SetNDC(); tag1.SetTextFont(42)
tag1.SetTextSize(0.045)
tag2 = ROOT.TLatex(cmsx1,cmsx2,"CMS")
tag2.SetNDC()
tag2.SetTextFont(62)
tag3 = ROOT.TLatex(prelx1,prelx2,"Preliminary")
tag3.SetNDC()
tag3.SetTextFont(52)
tag2.SetTextSize(0.055)
tag3.SetTextSize(0.045)
tag1.Draw()
tag2.Draw()
tag3.Draw()

tag4 = ROOT.TLatex(regionx1, regionx2, region)
tag4.SetNDC()
tag4.SetTextFont(42)
tag4.SetTextSize(0.045)

tag4.Draw()

#SAVE#######################################
c.SaveAs(tag+"_"+flavor+"_stack_all"+extension)
