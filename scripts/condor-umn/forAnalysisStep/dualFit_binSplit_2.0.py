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

#BINNING WORK
#OG binEdgeList = [800., 1000., 1200., 1500., 1800., 8000.]
#400 DIV
#binEdgeListDY = [800., 1200., 1600., 2000., 2400., 2800., 8000.]
#binningDY = array("d",binEdgeListDY)
#
#binEdgeListEMU = [800., 1200., 1600., 2000., 8000.]
#binningEMU = array("d",binEdgeListEMU)


def logHist(hist):
    newHist = copy.deepcopy(hist)
    newHist.Scale(0.0)
    for ibin in range(1, hist.GetNbinsX()+1):
        if hist.GetBinContent(ibin) == 0.0 or hist.GetBinError(ibin) == 0.0:
#            print "BAD BIN"
            binVal = 0.0
            binE   = 0.0
        else:
#            print "GOOD BIN"
#            print hist.GetBinContent(ibin)
#            print hist.GetBinError(ibin)
            binVal = log(hist.GetBinContent(ibin))
            binE   = log(hist.GetBinError(ibin))

        newHist.SetBinContent(ibin, binVal )
        newHist.SetBinError(ibin, binE )

    return newHist

#splits last bin up
def chiSquareCalc(X, mu, sigma):
    chi = (X - mu) / sigma
    return chi * chi
def chiSquared(hist, shape, Nparam):

    chi = 0.0
    N          = 0

    for ibin in range(1, hist.GetNbinsX()):
      X = hist.GetBinContent(ibin)  
      if (X <= 0.0):
          continue
      sigma = hist.GetBinError(ibin)
      mu = shape.GetBinContent(ibin)

      binChi = chiSquareCalc(X, mu, sigma)
      print "BIN "+str(ibin)+" CHISQUARED: "+str(binChi)
      chi += binChi
      N = N + 1

    if (N - Nparam) < 1:
        print "BAD CHI SQUARED: RETURNING 100000000"
        return 100000000

    return chi / float(N-Nparam)


def findNearBinCenter(val, hist):
    closest = 0
    for ibin in range(1, hist.GetNbinsX()+1):
        closest = ibin
        if ibin == hist.GetNbinsX():
            break
        distance = abs(val - hist.GetBinCenter(ibin))
        nextDistance = abs(val - hist.GetBinCenter(ibin+1))
        if not (nextDistance <= distance): #ERR ON THE HIGH SIDE
            break
    return closest
def addUncertainty(mainHist, uncHist):
    for ibin in range(1, mainHist.GetNbinsX()+1):
        binE = mainHist.GetBinError(ibin)
        binCenter = mainHist.GetBinCenter(ibin)
        nearBin = findNearBinCenter(binCenter, uncHist)
        addE = uncHist.GetBinError(nearBin)

        newE = math.sqrt(binE * binE + addE * addE)
            
        mainHist.SetBinError(ibin, newE)

def getHistoUp(mainHist):
    histoUp = copy.deepcopy(mainHist)
    histoUp.Scale(0.0)

    for ibin in range(1, mainHist.GetNbinsX()+1):
        binE = mainHist.GetBinError(ibin)
        binVal = mainHist.GetBinContent(ibin)

        histoUp.SetBinContent(ibin, binE+binVal)

    histoUp.SetLineStyle(9)
    #histoUp.SetLineWidth(2)

    return histoUp
        
def getHistoLow(mainHist):
    histoLow = copy.deepcopy(mainHist)
    histoLow.Scale(0.0)

    for ibin in range(1, mainHist.GetNbinsX()+1):
        binE = mainHist.GetBinError(ibin)
        binVal = mainHist.GetBinContent(ibin)

        lowVal = binVal - binE
        if lowVal < 0.0:
            lowVal = 0.0

        histoLow.SetBinContent(ibin, lowVal)

    histoLow.SetLineStyle(9)
    #histoLow.SetLineWidth(2)

    return histoLow

def fitIntegral(fitFunction, binning):
    binningArray = array("d",binning)
    hist = ROOT.TH1D("FitIntegral","Fit Integral", len(binning)-1,binningArray)
    for ibin in range(1, len(binning)):
        binC = fitFunction.Integral(      binning[ibin-1], binning[ibin])
        binE = fitFunction.IntegralError( binning[ibin-1], binning[ibin])
    
        binC /= (binning[ibin] - binning[ibin-1]) 
        binE /= (binning[ibin] - binning[ibin-1])
    
        print "FROM "+str(binning[ibin-1])+" to "+str(binning[ibin])
        print str(binC)+" +/- "+str(binE)
    
        hist.SetBinContent(ibin, binC)
        hist.SetBinError ( ibin, binE)



    return hist
            
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
    newHist.SetTitle("")
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
            print str(ibin)+": POSITIVE"
            negHist.SetBinContent(ibin, 0.0)
        elif negHist.GetBinContent(ibin) < 0.0:
            print str(ibin)+": NEGATIVE"
            negHist.SetBinContent(ibin, negHist.GetBinContent(ibin)*-1.0)
        else:
            print str(ibin)+": 0.0"

    return negHist


#############################################################################################
binEdgeListDY = [800., 1200., 1600., 2000., 2400., 2800., 8000.]
binningDY = array("d",binEdgeListDY)

binEdgeListDYsplit = [800., 1200., 1600., 2000., 2400., 2800.,4000., 8000.]
binningDYsplit = array("d",binEdgeListDYsplit)

binEdgeListEMU = [800., 1200., 1600., 8000.]
binningEMU = array("d",binEdgeListEMU)

binEdgeListEMUsplit = [800., 1200., 1600., 2000., 2400., 2800., 4000., 8000.]
binningEMUsplit = array("d",binEdgeListEMUsplit)

binEdgeList100 = []
for i in range(800, 8100, 400):
    binEdgeList100.append(i)
fineBinning = array("d", binEdgeList100)

lumi2016 = 35.920
lumi2017 = 41.530
lumi2018 = 59.740
lumiScale = 1.0

print "STARTING"

#GRABBING THE ANA HISTOS
#fileName = "WR_NR_MuMuJJ_ResolvedSignalRegion.root"
#fileName = "WR_NR_MuMuJJ_BoostedSignalRegion_400BINS.root"
year = "2016"
if year == "2018":
    lumiScale = 1.0/lumi2018
if year == "2017":
    lumiScale = 1.0/lumi2017
if year == "2016":
    lumiScale = 1.0/lumi2016

fileName = "analysis"+year+"/WR_NR_MuMuJJ_BoostedSignalRegion_400GeVBin.root"
output = "SoverB"
signal = "WR_5000_NR_200"
folder = "MuMuJJ/"

c = ROOT.TCanvas("c","c",2000,2500)
c.Divide(2,4)
#1,2
#3,4
#5,6

myOtherStack = ROOT.THStack()

fileA = ROOT.TFile.Open(fileName, "read")

for hist in otherbgs:
    myOtherStack.Add(fileA.Get(folder+hist))

#MAIN BGS
DYhist = fileA.Get(folder+"DY")
DYhist.SetTitle("")
DYhist.Scale(lumiScale)
negDY = negatives(DYhist)
bigBinDY = DYhist.Rebin(6, "", binningDY) #stat window binning
#bigBinDY = histBinSplit(bigBinDY, 2)
bigBinDY = histBinSplit(bigBinDY, 13)     #split into 400 GeV pieces
bigBinDY = bigBinDY.Rebin(len(binEdgeListDYsplit)-1 , "" , binningDYsplit)           #fancy analysis binning
DYhist = removeNegBins(DYhist)
#DYhist = SetEmptyBinErrorToOne(DYhist)
EmuHist = fileA.Get(folder+"EMu")
EmuHist.SetTitle("")
EmuHist.Scale(lumiScale)
negEMU = negatives(EmuHist)
bigBinEMU = EmuHist.Rebin(3, "", binningEMU) #stat window binning
#bigBinEMU = histBinSplit(bigBinEMU, 2)
bigBinEMU = histBinSplit(bigBinEMU, 16) #split into 400 GeV pieces
bigBinEMU = bigBinEMU.Rebin(len(binEdgeListEMUsplit)-1 , "" , binningEMUsplit)           #fancy analysis binning
EmuHist = removeNegBins(EmuHist)
#OTHER BGS
otherBGhist = histFromStack(myOtherStack)
newBGstack = ROOT.THStack() #DY, EMU, OTHER
bigBinBGhist = otherBGhist.Rebin(6, "",binningDY)
#TEMPLATES FOR LATER
DYtemplate  = copy.deepcopy(otherBGhist)
EMUtemplate = copy.deepcopy(otherBGhist)
DYtemplate.Scale( 0.0)
EMUtemplate.Scale(0.0)

#FIT FORMS
expForm = 'exp([0]-[1]*x)'
#expMod  = '[0]*exp(-((x-800.0)/([1]))^[2])'
expMod  = '[0]*exp(-(x-800.0)/([1]))'
expXpow  = '[0]*exp(-((x-800.0)^[2])/([1]))'
atlasFunc = '[0]*pow((1-(x/13000.0)),[1])*pow((1+(x/13000.0)),([2]*(x/13000.0)))'

#fitFormDY  = atlasFunc
fitFormDY  = expForm
#fitFormDY  = expXpow
fitFormEMU = expForm

#DY ONLY########################################################################
c.cd(1)
print "DY MINIMUM:"
print DYhist.GetMinimum()
print DYhist.GetBinCenter(DYhist.GetMinimumBin())
print DYhist.GetBinError(DYhist.GetMinimumBin())

ROOT.gPad.SetLogy()
DYhist.SetMinimum(0.00001)
DYhist.SetLineColor(ROOT.kBlue)
DYhist.SetTitle("DY")
DYhist.SetLineWidth(3)
DYhist.GetXaxis().SetTitle("M_{lj} (GeV)")
DYhist.GetYaxis().SetTitle("events / 400 GeV")

bigBinDY.SetLineColor(ROOT.kRed)

fitFunctionDY = ROOT.TF1('funcDY', fitFormDY, 800.0, 8000.0)

fitOptions = "IWLVRS"

print "FITTINGTIME"
myFitResult = DYhist.Fit("funcDY", fitOptions)
myFitResult.GetCovarianceMatrix().Print()
bigBinDY.Draw("esame")

print int(myFitResult)

print "###################DY PARAMETERS######################"
print str(fitFunctionDY.GetParameter(0))+" +/- "+str(fitFunctionDY.GetParError(0))
print str(fitFunctionDY.GetParameter(1))+" +/- "+str(fitFunctionDY.GetParError(1))

print "DONEDONEDONE"

DYtemplate     = fitIntegral(fitFunctionDY, binEdgeListDYsplit)
DYtemplateFINE = fitIntegral(fitFunctionDY, binEdgeList100)

Nparam = 2 
fitChiDY = chiSquared(DYhist, DYtemplateFINE, Nparam)
print "DY FIT CHI SQUARED: "+str(fitChiDY)

lowEdgeHist = getHistoLow(DYtemplateFINE)
highEdgeHist = getHistoUp(DYtemplateFINE)

lowEdgeHist.Draw("Csame")
highEdgeHist.Draw("Csame")

newLegDY = ROOT.TLegend(0.55,0.6,1.0,1.0)
newLegDY.AddEntry(DYhist, "DY background")
newLegDY.AddEntry(bigBinDY,"Big Bin DY")
newLegDY.AddEntry(lowEdgeHist,"Fit Par. Unc. Envelope")
DYp0 = "{:.1f}".format(fitFunctionDY.GetParameter(0))
DYp0E = "{:.1f}".format(fitFunctionDY.GetParError(0))
DYp1 = "{:.4f}".format(fitFunctionDY.GetParameter(1))
DYp1E = "{:.4f}".format(fitFunctionDY.GetParError(1))
DYp0_str = DYp0+"+/-"+DYp0E
DYp1_str = DYp1+"+/-"+DYp1E
fitStrDY = "e^{A-Bx}; A="+DYp0_str+" ;B="+DYp1_str
newLegDY.AddEntry(DYhist, fitStrDY)

newLegDY.Draw()

DYpaveChi = ROOT.TPaveText(0.7,0.5,1.0,0.58, "NDC")
DYchiText = "\chi^{2} = "+"{:.2f}".format(fitChiDY)
DYpaveChi.AddText(DYchiText)

DYpaveChi.Draw()

#EMU ONLY#########################################################
c.cd(2)
print ""
print "Emu MINIMUM:"
print EmuHist.GetMinimum()
print EmuHist.GetBinCenter(EmuHist.GetMinimumBin())

ROOT.gPad.SetLogy()
EmuHist.SetMinimum(0.00001)
EmuHist.SetLineColor(ROOT.kGreen)
EmuHist.SetTitle("E-Mu")
EmuHist.SetLineWidth(3)
EmuHist.GetXaxis().SetTitle("M_{lj} (GeV)")
EmuHist.GetYaxis().SetTitle("events / 400 GeV")

bigBinEMU.SetLineColor(ROOT.kRed)

fitFunctionEMU = ROOT.TF1('funcEMU', fitFormEMU, 800, 8000)

fitResultEmu = EmuHist.Fit("funcEMU", fitOptions)
bigBinEMU.Draw("esame")

EMUtemplate     = fitIntegral(fitFunctionEMU, binEdgeListEMUsplit)
EMUtemplateFINE = fitIntegral(fitFunctionEMU, binEdgeList100)

fitChiEMU = chiSquared(EmuHist, EMUtemplateFINE, Nparam)

lowEdgeHistEMU = getHistoLow(EMUtemplateFINE)
highEdgeHistEMU = getHistoUp(EMUtemplateFINE)

lowEdgeHistEMU.Draw("Csame")
highEdgeHistEMU.Draw("Csame")

newLegEMU = ROOT.TLegend(0.55,0.6,1.0,1.0)
newLegEMU.AddEntry(EmuHist, "EMu background")
newLegEMU.AddEntry(bigBinEMU,"Big Bin Emu")
newLegEMU.AddEntry(lowEdgeHist,"Fit Par. Unc. Envelope")
EMUp0 = "{:.1f}".format(fitFunctionEMU.GetParameter(0))
EMUp0E = "{:.1f}".format(fitFunctionEMU.GetParError(0))
EMUp1 = "{:.4f}".format(fitFunctionEMU.GetParameter(1))
EMUp1E = "{:.4f}".format(fitFunctionEMU.GetParError(1))
EMUp0_str = EMUp0+"+/-"+EMUp0E
EMUp1_str = EMUp1+"+/-"+EMUp1E
fitStrEMU = "e^{A-Bx}; A="+EMUp0_str+" ;B="+EMUp1_str
newLegEMU.AddEntry(EmuHist, fitStrEMU)

newLegEMU.Draw()

#EMUpaveChi = ROOT.TPaveText(0.7,0.37, 0.9,0.57)
EMUpaveChi = ROOT.TPaveText(0.7,0.5,1.0,0.58, "NDC")
EMUchiText = "\chi^{2} = "+"{:.2f}".format(fitChiEMU)
EMUpaveChi.AddText(EMUchiText)

EMUpaveChi.Draw()

#CONSTRUCTING THE TOTAL PICTURE#######################################

#ADDING UNCERTAINTIES
addUncertainty(DYtemplate, bigBinDY)
addUncertainty(EMUtemplate, bigBinEMU)

#HISTS MADE TIME TO STACK
DYtemplate.SetLineColor(ROOT.kBlue)
newBGstack.Add(DYtemplate)
EMUtemplate.SetLineColor(ROOT.kGreen)
newBGstack.Add(EMUtemplate)
otherBGhist.SetLineColor(ROOT.kBlack)
newBGstack.Add(otherBGhist)


c.cd(3)
ROOT.gPad.SetLogy()
DYtemplate.SetLineWidth(3)
DYtemplate.Draw("esame")
DYtemplate.SetTitle("\mathrm{DY\ fit} \otimes \mathrm{MC\ stat.\ unc.}")
DYtemplate.GetXaxis().SetTitle("M_{lj} (GeV)")
DYtemplate.GetYaxis().SetTitle("events / 400 GeV")
#otherBGhist.Draw("esame")

#newLegDY = ROOT.TLegend(0.7,0.7,1.0,1.0)
#newLegDY.AddEntry(DYtemplate, "DY FIT")
#newLeg.AddEntry(otherBGhist, "Other BGs")
#newLegDY.Draw()
c.cd(4)
ROOT.gPad.SetLogy()
EMUtemplate.SetTitle("")
EMUtemplate.Draw("e")
EMUtemplate.SetLineWidth(2)
EMUtemplate.SetTitle("\mathrm{E-Mu\ fit} \otimes \mathrm{MC\ stat.\ unc.}")
EMUtemplate.GetXaxis().SetTitle("M_{lj} (GeV)")
EMUtemplate.GetYaxis().SetTitle("events / 400 GeV")
#otherBGhist.Draw("esame")

#newLegEMU = ROOT.TLegend(0.7,0.7,1.0,1.0)
#newLegEMU.AddEntry(EMUtemplate, "EMU FIT")
#newLeg.AddEntry(otherBGhist, "Other BGs")
#newLegEMU.Draw()
#ROOT.gPad.SetLogy()

c.cd(5)

negDY.GetXaxis().SetTitle("M_{lj} (GeV)")
negDY.GetYaxis().SetTitle("events / 400 GeV")
negDY.Draw("e")
negDY.SetLineColor(ROOT.kRed)
negDY.SetLineWidth(3)
ROOT.gPad.SetLogy()
DYhist.Draw("esame")

DYnegLeg = ROOT.TLegend(0.6,0.7,1.0,1.0)
DYnegLeg.AddEntry(DYhist, "DY background")
DYnegLeg.AddEntry(negDY, "DY (inverted) neg. bins")

DYnegLeg.Draw()

c.cd(6)
negEMU.GetXaxis().SetTitle("M_{lj} (GeV)")
negEMU.GetYaxis().SetTitle("events / 400 GeV")
negEMU.Draw("e")
negEMU.SetLineColor(ROOT.kRed)
negEMU.SetLineWidth(3)
ROOT.gPad.SetLogy()
EmuHist.Draw("esame")

EMUnegLeg = ROOT.TLegend(0.6,0.7,1.0,1.0)
EMUnegLeg.AddEntry(EmuHist, "EMU background")
EMUnegLeg.AddEntry(negEMU, "EMU (inverted) neg. bins")

EMUnegLeg.Draw()

c.SaveAs(output+"_ratioPlot"+year+".png")

print "DY FIT CHI SQUARED: "+str(fitChiDY)
print "DY FIT CHI SQUARED: "+str(fitChiEMU)
