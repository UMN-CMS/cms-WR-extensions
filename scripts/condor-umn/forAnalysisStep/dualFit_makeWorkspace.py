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

fittedbgs = [
"DY",
"EMu"
]


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
def addUncertainty(mainHist, uncHist, scale=1.0):
    print "mainHist.GetName(): ", mainHist.GetName()
    for ibin in range(1, mainHist.GetNbinsX()+1):
        binE = mainHist.GetBinError(ibin)
        binCenter = mainHist.GetBinCenter(ibin)
        nearBin = findNearBinCenter(binCenter, uncHist)
        addE = uncHist.GetBinError(nearBin)
#	if mainHist.GetName() == "DY" and ibin > 5:
#	    addE = addE + addE*15
#        elif mainHist.GetName() == "EMu" and ibin > 3:
#            addE = addE + addE*15
	print "addE*scale: "+str(addE)+"*"+str(scale)
        addE *= scale

        newE = math.sqrt(binE * binE + addE * addE)
            
        mainHist.SetBinError(ibin, newE)

def getHistoUp(mainHist):
    histoUp = copy.deepcopy(mainHist)
    histoUp.Scale(0.0)

    for ibin in range(1, mainHist.GetNbinsX()+1):
        binE = mainHist.GetBinError(ibin)
        binVal = mainHist.GetBinContent(ibin)

        histoUp.SetBinContent(ibin, binE+binVal)

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

    return histoLow
            
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
	print "hist.GetBinContent(ibin): ", hist.GetBinContent(ibin)
	print "hist.GetBinError(ibin): ", hist.GetBinError(ibin)
        newBinning.append(hist.GetBinLowEdge(ibin))

    lastBinUpper   = hist.GetBinWidth(NoldBins) + hist.GetBinLowEdge(ibin)
    lastBinContent = hist.GetBinContent(NoldBins)
    lastBinError   = hist.GetBinError(  NoldBins) 

    splitContent = float(lastBinContent) / float(NlastBins)
    splitError   = float(lastBinError)   / math.sqrt(float(NlastBins))

    print "splitError: ", splitError

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

def createSystematicTemplate(DYhisto, DYhistoBigBin, Systhisto):
    print "CREATING A SYST TEMPLATE"

    print "bins in systhisto: ", Systhisto.GetNbinsX()
    print "bins in DY big bin: ", DYhistoBigBin.GetNbinsX()
    for ibin in range(1, Systhisto.GetNbinsX()+1):
	print "Systhisto bin content: ", Systhisto.GetBinContent(ibin)
        print "DYhistoBigBin bin content: ", DYhistoBigBin.GetBinContent(ibin)


    Systhisto.Divide(DYhistoBigBin)
    for ibin in range(1, Systhisto.GetNbinsX()+1):
        print "Systhisto bin content: ", Systhisto.GetBinContent(ibin)

    
    SystTemplate = copy.deepcopy(DYhisto)
    for ibin in range(1, DYhisto.GetNbinsX()+1):
	print "Systhisto.GetBinContent(Systhisto.GetXaxis().FindBin(DYhisto.GetBinCenter(ibin)))): ", Systhisto.GetBinContent(Systhisto.GetXaxis().FindBin(DYhisto.GetBinCenter(ibin)))
	SystTemplate.SetBinContent(ibin, DYhisto.GetBinContent(ibin)*Systhisto.GetBinContent(Systhisto.GetXaxis().FindBin(DYhisto.GetBinCenter(ibin))))

    return SystTemplate

#############################################################################################
#WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root
#SingleMuon_WR_M-2400_LNu_M-240.root

print "STARTING"

if len(sys.argv) == 3 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    print "WRONG"
    print "Specify the workspace folder and the year!"
    exit(0)

workspaceFolder = sys.argv[1]
year = sys.argv[2]
if(len(sys.argv) == 4):
    scale = float(sys.argv[3])
else:
    scale = 1.0


#BINNING WORK
#OG binEdgeList = [800., 1000., 1200., 1500., 1800., 8000.]
#binning = array("d",binEdgeList)

binEdgeListDY = [800., 1200., 1600., 2000., 2400., 2800., 8000.]
binningDY = array("d",binEdgeListDY)
nBinsDY = len(binningDY) - 1

binEdgeListEMU = [800., 1200., 1600., 2000., 8000.]
binningEMU = array("d",binEdgeListEMU)
nBinsEMU = len(binningEMU) - 1

#histoPassBoostOff =  histoPassBoostOff.Rebin(5,"", binning)

#GRABBING THE ANA HISTOS
fileName = "WR_NR_MuMuJJ_BoostedSignalRegion_400GeVBins.root"
#fileName = "WR_NR_MuMuJJ_BoostedSignalRegion_400BINS.root"
outputFileName = "WR_NR_MuMuJJ_BoostedSignalRegion.root"

fileName = workspaceFolder +"/"+ fileName
outputFileName = workspaceFolder +"/"+ outputFileName

output = "fitted"
signal = "WR_5000_NR_200"
folder = "MuMuJJ/"

c = ROOT.TCanvas("c","c",1000,1500)
c.Divide(1,2)
#1,2
#3,4
#5,6

myStackA = ROOT.THStack()
myOtherStack = ROOT.THStack()

fileA = ROOT.TFile.Open(fileName, "read")
fileB = ROOT.TFile.Open(outputFileName, "recreate")

MuMuJJ = fileB.mkdir("MuMuJJ")
#MuMuJJ.cd()

for hist in bgs:
    myStackA.Add(fileA.Get(folder+hist))
for hist in otherbgs:
    myOtherStack.Add(fileA.Get(folder+hist))

MuMuJJ.cd()
for key in fileA.GetListOfKeys():
    print "key: ", key
    if key.IsFolder():
	print "Inside IsFolder"
	dir = fileA.Get(key.GetName())
	for hist in dir.GetListOfKeys():
	    print "key: ", hist
#            if hist.GetName() in bgs:
    	    if hist.GetName() in fittedbgs:
	    	continue
	    elif "DY" in hist.GetName():
		continue
            elif "EMu" in hist.GetName():
                continue
    	    else:
            	fileA.Get(folder+hist.GetName()).Write()


#MAIN BGS
DYhist = fileA.Get(folder+"DY")
EmuHist = fileA.Get(folder+"EMu")
SThist = fileA.Get(folder+"ST")
DiBosonHist = fileA.Get(folder+"DiBoson")
TriBosonHist = fileA.Get(folder+"TriBoson")
ttVhist = fileA.Get(folder+"ttV")
WJetsHist = fileA.Get(folder+"WJets")

#DY SYSTEMATICS
DYhist_JECUp = fileA.Get(folder+"DY_JEC_%sUp"%(year))
DYhist_JECDown = fileA.Get(folder+"DY_JEC_%sDown"%(year))
DYhist_JERUp = fileA.Get(folder+"DY_JER_%sUp"%(year))
DYhist_JERDown = fileA.Get(folder+"DY_JER_%sDown"%(year))
DYhist_PUUp = fileA.Get(folder+"DY_PU_%sUp"%(year))
DYhist_PUDown = fileA.Get(folder+"DY_PU_%sDown"%(year))
DYhist_MuIDUp = fileA.Get(folder+"DY_MuID_%sUp"%(year))
DYhist_MuIDDown = fileA.Get(folder+"DY_MuID_%sDown"%(year))
DYhist_MuResolUp = fileA.Get(folder+"DY_MuResol_%sUp"%(year))
DYhist_MuResolDown = fileA.Get(folder+"DY_MuResol_%sDown"%(year))
DYhist_MuTrigUp = fileA.Get(folder+"DY_MuTrig_%sUp"%(year))
DYhist_MuTrigDown = fileA.Get(folder+"DY_MuTrig_%sDown"%(year))
DYhist_LSFUp = fileA.Get(folder+"DY_LSF_%sUp"%(year))
DYhist_LSFDown = fileA.Get(folder+"DY_LSF_%sDown"%(year))
DYhist_MuZweightUp = fileA.Get(folder+"DY_Zweight_%sUp"%(year))
DYhist_MuZweightDown = fileA.Get(folder+"DY_Zweight_%sDown"%(year))

#EMu SYSTEMATICS
#EMuhist_SystUp = fileA.Get(folder+"EMu_Syst_%sUp"%(year))
#EMuhist_SystDown = fileA.Get(folder+"EMu_Syst_%sDown"%(year))


#MAKE ENVELOPE HISTOGRAMS
bigBinDY = DYhist.Rebin(nBinsDY, "", binningDY)
bigBinDYsplit = histBinSplit(bigBinDY, 13)
bigBinDY.SetName("DY Big")

bigBinEMU = EmuHist.Rebin(nBinsEMU, "", binningEMU)
bigBinEMUsplit = histBinSplit(bigBinEMU, 15)
bigBinEMU.SetName("EMU Big")

#REMOVE NEGATIVE CONTENT FROM FINE HISTOGRAMS
EmuHist = removeNegBins(EmuHist)
DYhist  = removeNegBins(DYhist)

#TEMPLATES FOR LATER
DYtemplate  = copy.deepcopy(DYhist)
DYtemplate.SetName("DY Template")
DYtemplate.Scale( 0.0)

EMUtemplate = copy.deepcopy(EmuHist)
EMUtemplate.SetName("EMU Template")
EMUtemplate.Scale(0.0)


#FIT FORMS
expForm = 'exp([0]-[1]*x)'
expMod  = '[0]*exp(-(x-800.0)/([1]))'
expXpow  = '[0]*exp(-((x-800.0)^[2])/([1]))'
atlasFunc = '[0]*pow((1-(x/13000.0)),[1])*pow((1+(x/13000.0)),([2]*(x/13000.0)))'

fitFormDY  = expForm
fitFormEMU = expForm

########################################################DY FITTING########################################################################
c.cd(2)
print "DY MINIMUM:"
print DYhist.GetMinimum()
print DYhist.GetBinCenter(DYhist.GetMinimumBin())
print DYhist.GetBinError(DYhist.GetMinimumBin())

ROOT.gPad.SetLogy()
#DYhist = logHist(DYhist)
DYhist.SetMinimum(0.00000001)
DYhist.SetLineColor(ROOT.kBlue)
DYhist.SetTitle("DY")
DYhist.SetLineWidth(3)
#DYhist.Draw("e")



fitFunctionDY = ROOT.TF1('funcDY', fitFormDY, 800.0, 8000.0)

fitOptions = "IWLVRS"

print "FITTINGTIME"
myFitResult = DYhist.Fit("funcDY", fitOptions)
myFitResult.GetCovarianceMatrix().Print()

print int(myFitResult)

print "###################DY PARAMETERS######################"
print str(fitFunctionDY.GetParameter(0))+" +/- "+str(fitFunctionDY.GetParError(0))
print str(fitFunctionDY.GetParameter(1))+" +/- "+str(fitFunctionDY.GetParError(1))

print "DONEDONEDONE"
for ibin in range(1, DYhist.GetNbinsX()+1):
    DYbin  = fitFunctionDY.Integral( DYhist.GetBinLowEdge(ibin), DYhist.GetBinLowEdge(ibin+1))
    DYbinE = fitFunctionDY.IntegralError( DYhist.GetBinLowEdge(ibin), DYhist.GetBinLowEdge(ibin+1))

    DYbin  /= DYhist.GetBinWidth(ibin)
    DYbinE /= DYhist.GetBinWidth(ibin)

    print "FROM "+str(DYhist.GetBinLowEdge(ibin))+" to "+str(DYhist.GetBinLowEdge(ibin+1))
    print str(DYbin)+" +/- "+str(DYbinE)

    DYtemplate.SetBinContent(ibin, DYbin)
    DYtemplate.SetBinError( ibin, DYbinE)

Nparam = 2 
fitChi = chiSquared(DYhist, DYtemplate, Nparam)
print "DY FIT CHI SQUARED: "+str(fitChi)

#####################################################EMU FITTING#########################################################
c.cd(4)
print ""
print "Emu MINIMUM:"
print EmuHist.GetMinimum()
print EmuHist.GetBinCenter(EmuHist.GetMinimumBin())

ROOT.gPad.SetLogy()
#EmuHist = logHist(EmuHist)
EmuHist.SetMinimum(0.000000001)
EmuHist.SetLineColor(ROOT.kGreen)
EmuHist.SetTitle("EMU")
EmuHist.SetLineWidth(3)
#EmuHist.Draw("e")

fitFunctionEMU = ROOT.TF1('funcEMU', fitFormEMU, 800, 8000)

fitResultEmu = EmuHist.Fit("funcEMU", fitOptions)

for ibin in range(1, EmuHist.GetNbinsX()+1):
    EMUbin  = fitFunctionEMU.Integral( EmuHist.GetBinLowEdge(ibin), EmuHist.GetBinLowEdge(ibin+1))
    EMUbinE = fitFunctionEMU.IntegralError( EmuHist.GetBinLowEdge(ibin), EmuHist.GetBinLowEdge(ibin+1))

    EMUbin  /= EmuHist.GetBinWidth(ibin)
    EMUbinE /= EmuHist.GetBinWidth(ibin)

    print "FROM "+str(EmuHist.GetBinLowEdge(ibin))+" to "+str(EmuHist.GetBinLowEdge(ibin+1))
    print str(EMUbin)+" +/- "+str(EMUbinE)

    EMUtemplate.SetBinContent(ibin, EMUbin)
    EMUtemplate.SetBinError( ibin, EMUbinE)

#CONSTRUCTING THE TOTAL PICTURE#######################################

#ADDING UNCERTAINTIES
print "Adding DY unc"
addUncertainty(DYtemplate, bigBinDYsplit, scale)
print "Adding ttbar unc"
addUncertainty(EMUtemplate, bigBinEMUsplit, scale)

print "TEMPLATES DONE!"

####CREATING DY SYSTEMATIC HISTOGRAMS#############################
DYhist_JECUp_temp = DYhist_JECUp.Rebin(nBinsDY, "", binningDY)
DYhist_JECDown_temp = DYhist_JECDown.Rebin(nBinsDY, "", binningDY)
DYhist_JERUp_temp = DYhist_JERUp.Rebin(nBinsDY, "", binningDY)
DYhist_JERDown_temp = DYhist_JERDown.Rebin(nBinsDY, "", binningDY)
DYhist_PUUp_temp = DYhist_PUUp.Rebin(nBinsDY, "", binningDY)
DYhist_PUDown_temp = DYhist_PUDown.Rebin(nBinsDY, "", binningDY)
DYhist_MuIDUp_temp = DYhist_MuIDUp.Rebin(nBinsDY, "", binningDY)
DYhist_MuIDDown_temp = DYhist_MuIDDown.Rebin(nBinsDY, "", binningDY)
DYhist_MuResolUp_temp = DYhist_MuResolUp.Rebin(nBinsDY, "", binningDY)
DYhist_MuResolDown_temp = DYhist_MuResolDown.Rebin(nBinsDY, "", binningDY)
DYhist_MuTrigUp_temp = DYhist_MuTrigUp.Rebin(nBinsDY, "", binningDY)
DYhist_MuTrigDown_temp = DYhist_MuTrigDown.Rebin(nBinsDY, "", binningDY)
DYhist_MuZweightUp_temp = DYhist_MuZweightUp.Rebin(nBinsDY, "", binningDY)
DYhist_MuZweightDown_temp = DYhist_MuZweightDown.Rebin(nBinsDY, "", binningDY)
DYhist_LSFUp_temp = DYhist_LSFUp.Rebin(nBinsDY, "", binningDY)
DYhist_LSFDown_temp = DYhist_LSFDown.Rebin(nBinsDY, "", binningDY)

#EMuhist_SystUp_temp = EMuhist_SystUp.Rebin(nBinsEMU, "", binningEMU)
#EMuhist_SystDown_temp = EMuhist_SystDown.Rebin(nBinsEMU, "", binningEMU)

DY_JECUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_JECUp_temp)
DY_JECDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_JECDown_temp)
DY_JERUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_JERUp_temp)
DY_JERDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_JERDown_temp)
DY_PUUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_PUUp_temp)
DY_PUDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_PUDown_temp)
DY_MuResolUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuResolUp_temp)
DY_MuResolDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuResolDown_temp)
DY_MuTrigUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuTrigUp_temp)
DY_MuTrigDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuTrigDown_temp)
DYhist_MuZweightUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuZweightUp_temp)
DYhist_MuZweightDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuZweightDown_temp)
DY_MuIDUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuIDUp_temp)
DY_MuIDDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_MuIDDown_temp)
DY_LSFUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_LSFUp_temp)
DY_LSFDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY, DYhist_LSFDown_temp)

#EMu_SystUpTemplate = createSystematicTemplate(EMUtemplate, bigBinEMU, EMuhist_SystUp_temp)
#EMu_SystDownTemplate = createSystematicTemplate(EMUtemplate, bigBinEMU, EMuhist_SystDown_temp)

DY_JECUpTemplate.SetName("DY_JEC_%sUp"%(year))
DY_JECDownTemplate.SetName("DY_JEC_%sDown"%(year))
DY_JERUpTemplate.SetName("DY_JER_%sUp"%(year))
DY_JERDownTemplate.SetName("DY_JER_%sDown"%(year))
DY_PUUpTemplate.SetName("DY_PU_%sUp"%(year))
DY_PUDownTemplate.SetName("DY_PU_%sDown"%(year))
DY_MuResolUpTemplate.SetName("DY_MuResol_%sUp"%(year))
DY_MuResolDownTemplate.SetName("DY_MuResol_%sDown"%(year))
DY_MuTrigUpTemplate.SetName("DY_MuTrig_%sUp"%(year))
DY_MuTrigDownTemplate.SetName("DY_MuTrig_%sDown"%(year))
DYhist_MuZweightUpTemplate.SetName("DY_Zweight_%sUp"%(year))
DYhist_MuZweightDownTemplate.SetName("DY_Zweight_%sDown"%(year))
DY_MuIDUpTemplate.SetName("DY_MuID_%sUp"%(year))
DY_MuIDDownTemplate.SetName("DY_MuID_%sDown"%(year))
DY_LSFUpTemplate.SetName("DY_LSF_%sUp"%(year))
DY_LSFDownTemplate.SetName("DY_LSF_%sDown"%(year))

#EMu_SystUpTemplate.SetName("EMu_Syst_%sUp"%(year))
#EMu_SystDownTemplate.SetName("EMu_Syst_%sDown"%(year))

DY_JECUpTemplate.Write()
DY_JECDownTemplate.Write()
DY_JERUpTemplate.Write()
DY_JERDownTemplate.Write()
DY_PUUpTemplate.Write()
DY_PUDownTemplate.Write()
DY_MuIDUpTemplate.Write()
DY_MuIDDownTemplate.Write()
DY_MuResolUpTemplate.Write()
DY_MuResolDownTemplate.Write()
DY_MuTrigUpTemplate.Write()
DY_MuTrigDownTemplate.Write()
DYhist_MuZweightUpTemplate.Write()
DYhist_MuZweightDownTemplate.Write()
DY_LSFUpTemplate.Write()
DY_LSFDownTemplate.Write()

#EMu_SystUpTemplate.Write()
#EMu_SystDownTemplate.Write()

#SAVING DY AND EMU################################################
DYtemplate.SetName("DY")
EMUtemplate.SetName("EMu")
SThist.SetName("ST")
DiBosonHist.SetName("DiBoson")
TriBosonHist.SetName("TriBoson")
ttVhist.SetName("ttV")
WJetsHist.SetName("WJets")

DYtemplate.Write()
EMUtemplate.Write()
#SThist.Write()
#DiBosonHist.Write()
#TriBosonHist.Write()
#ttVhist.Write()
#WJetsHist.Write()
##################################################################

c.SaveAs(output+"_BGs.png")
