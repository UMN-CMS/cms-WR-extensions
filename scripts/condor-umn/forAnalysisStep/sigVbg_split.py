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
#import tdrstyle




#tdrstyle.setTDRStyle()
ROOT.gStyle.SetPadTopMargin(0.10)
ROOT.gStyle.SetPadLeftMargin(0.16)
ROOT.gStyle.SetPadRightMargin(0.10)
ROOT.gStyle.SetPaintTextFormat("1.1f")
ROOT.gStyle.SetOptFit(0000)
ROOT.gStyle.SetOptStat(0)
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

bgs = [
"DiBoson",
"TriBoson",
"ttV",
"ST",
"WJets",
"DY",
"EMu"
]

otherbgs = [
"DiBoson",
"TriBoson",
"ttV",
"ST",
"WJets",
]

            
def histBinSplit(hist, NlastBins):
    print "BIN PARTY"
    NoldBins = hist.GetNbinsX()
    print NoldBins
    NnewBins = NoldBins + NlastBins

    newBinning = []
    #get bin edges
    print "OLD BINS"
    for ibin in range(1,NoldBins+1):
        print ibin
        print hist.GetBinWidth(ibin)
        newBinning.append(hist.GetBinLowEdge(ibin))

    lastBinUpper   = hist.GetBinWidth(NoldBins) + hist.GetBinLowEdge(ibin)
    lastBinContent = hist.GetBinContent(NoldBins)
    lastBinError   = hist.GetBinError(  NoldBins) 

    splitContent = float(lastBinContent) / float(NlastBins)
    splitError   = float(lastBinError)   / math.sqrt(float(NlastBins))

    newBinsWidth = hist.GetBinWidth(NoldBins) / NlastBins

    print newBinning
    print newBinsWidth
    print "NEW BINS"
    for ibin in range(int(newBinning[-1]+newBinsWidth),int(lastBinUpper)+1,int(newBinsWidth)):
        print ibin
        newBinning.append(float(ibin))

    bins = array("d",newBinning)
    print newBinning

    newHist = ROOT.TH1D(hist.GetName(),hist.GetTitle(),len(newBinning)-1,bins)

    #fill it up
    print "FILL OLD"
    for ibin in range(1,NoldBins):
        print ibin
        newHist.SetBinContent(ibin, hist.GetBinContent(ibin))
        newHist.SetBinError(  ibin, hist.GetBinError(  ibin))

    print "FILL NEW"
    for ibin in range(NoldBins,len(newBinning)):
        print ibin
        print splitContent
        newHist.SetBinContent(ibin, splitContent)
        newHist.SetBinError(  ibin, splitError)


    return newHist
    

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


def SetEmptyBinErrorToOne(oldHist):
    hist = copy.deepcopy(oldHist)
    print "HOLA"
    for ibin in range(1, hist.GetNbinsX()+1):
        if hist.GetBinContent(ibin) == 0.0:
            print "FOUND ONE"
            hist.SetBinError(ibin, 1.0)

    print "HEREYAGO"
    return hist

def removeNegBins(oldHist):
    hist = copy.deepcopy(oldHist)
    removed = 0.0
    for ibin in range(1, hist.GetNbinsX()+1):
        if hist.GetBinContent(ibin) < 0.0:
            print "FOUND ONE"
            removed += hist.GetBinContent(ibin)
            hist.SetBinContent(ibin, 0.0)


    print "REMOVED: "+str(removed)
    return hist

def negatives(hist):
    negHist = copy.deepcopy(hist)

    for ibin in range(1, negHist.GetNbinsX()+1):
        if negHist.GetBinContent(ibin) > 0.0:
            negHist.SetBinContent(ibin, 0.0)
        elif negHist.GetBinContent(ibin) < 0.0:
            negHist.SetBinContent(ibin, negHist.GetBinContent(ibin)*-1.0)

    return negHist


#############################################################################################
print "STARTING"

binEdgeListEMU = [800., 1200., 1600., 2000., 2400., 8000.]
binningEMU = array("d",binEdgeListEMU)

binEdgeListTrad = [800., 1000., 1200., 1500., 1800., 8000.]
binningTrad = array("d", binEdgeListTrad)

binning       = binningEMU
nbins = len(binEdgeListEMU)-1

###
year="2016"
###
fileName = "analysis"+year+"/WR_NR_MuMuJJ_BoostedSignalRegion_400GeVBin.root"
#fileName = "analysis"+year+"/TradBinning.root"
output = "SoverB"
signal = "WR_5000_NR_200"
folder = "MuMuJJ/"
sigxsec = 0.00027040 #pb 5000 GeV NLO
defaultxsec = 0.001 #pb, 1fb

sigscale = sigxsec / defaultxsec

c = ROOT.TCanvas("c","c",1000,1000)

#GRABBING THE ANA HISTOS
myOtherStack = ROOT.THStack()

fileA = ROOT.TFile.Open(fileName, "read")

for hist in otherbgs:
    myOtherStack.Add(fileA.Get(folder+hist))

sigHist = fileA.Get(folder+signal)
#SIGNAL
sigHist.Scale(sigscale)

#MAIN BGS
DYhist = fileA.Get(folder+"DY")
#DYhist = removeNegBins(DYhist)

EMUhist = fileA.Get(folder+"EMu")
#EmuHist = removeNegBins(EmuHist)

#OTHER BGS
otherBGhist = histFromStack(myOtherStack)

#COMBINE
DYbig = DYhist
DYbig = DYhist.Rebin(nbins, "",binning)
DYbig.SetLineColor(ROOT.kBlue)
DYbig.SetFillColor(ROOT.kBlue)
DYbig.SetTitle("")
EMUbig = EMUhist
EMUbig = EMUhist.Rebin(nbins, "",binning)
EMUbig.SetLineColor(ROOT.kGreen)
EMUbig.SetFillColor(ROOT.kGreen)
EMUbig.SetTitle("")
otherBGbig = otherBGhist
otherBGbig = otherBGhist.Rebin(nbins, "",binning)
otherBGbig.SetLineColor(ROOT.kGray)
otherBGbig.SetFillColor(ROOT.kGray)
otherBGbig.SetTitle("")

bgStack = ROOT.THStack()
bgStack.Add(otherBGbig)
bgStack.Add(DYbig)
bgStack.Add(EMUbig)

bgHist = histFromStack(bgStack)
bgHistSplit = histBinSplit(bgHist, 2)
#bgHistSplit = histBinSplit(bgHist, 14)

#sigHist = sigHist.Rebin(nbins, "", binning)
sigHist.SetLineStyle(9)
sigHist.SetLineColor(ROOT.kBlack)
sigHist.SetLineWidth(3)

ROOT.gPad.SetLogy()

bgHistSplit.SetLineWidth(3)
bgHistSplit.SetLineColor(ROOT.kBlue)
bgHistSplit.SetMarkerStyle(5)
bgHistSplit.SetMarkerSize(3)
bgHistSplit.SetMarkerColor(ROOT.kBlue)
bgHistSplit.Draw("e")
bgHistSplit.GetYaxis().SetTitle("expected events ("+year+") per bin")
bgHistSplit.GetXaxis().SetTitle("M_{lj} (GeV)")

bgHist.SetLineWidth(3)
bgHist.SetLineColor(ROOT.kRed)
bgHist.Draw("esame")
bgHist.GetYaxis().SetTitle("expected events ("+year+") per bin")
bgHist.GetXaxis().SetTitle("M_{lj} (GeV)")


sigHist.Draw("samehist")

#LEGEND
leg = ROOT.TLegend(0.3,0.66,0.95,0.95)
leg.AddEntry(bgHist, "Total background")
leg.AddEntry(bgHistSplit, "Total background - Split")
leg.AddEntry(sigHist, "Signal M_{WR}: 5000 GeVM_{NR}: 200 GeV")

leg.Draw()

c.SaveAs("bg_v_sig"+year+"SPLIT.png")
