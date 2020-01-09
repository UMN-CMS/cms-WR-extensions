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

systematics = [
"JEC",
"JER",
"PU",
"MuID",
"MuResolUp",
"MuTrig",
"LSF"
]

if len(sys.argv) == 2 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    print "WRONG"
    exit(0)

isResolved = sys.argv[1]


#BINNING WORK
#OG binEdgeList = [800., 1000., 1200., 1500., 1800., 8000.]
#binning = array("d",binEdgeList)

if isResolved == "True":
  binEdgeListDY = [800., 1200., 1600., 2000., 2400., 2800., 8000.]
  binningDY = array("d",binEdgeListDY)
  nBinsDY = len(binningDY) - 1

  binEdgeListEMU = [800., 1200., 1600., 2000., 2400., 8000.]
  binningEMU = array("d",binEdgeListEMU)
  nBinsEMU = len(binningEMU) - 1

else:
  binEdgeListDY = [800., 1200., 1600., 2000., 2400., 2800., 8000.]
  binningDY = array("d",binEdgeListDY)
  nBinsDY = len(binningDY) - 1

  binEdgeListEMU = [800., 1200., 1600., 2000., 8000.]
  binningEMU = array("d",binEdgeListEMU)
  nBinsEMU = len(binningEMU) - 1

#histoPassBoostOff =  histoPassBoostOff.Rebin(5,"", binning)

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

#GRABBING THE ANA HISTOS
if isResolved == "True":
    fileName = "WR_NR_MuMuJJ_ResolvedSignalRegion_400GeVBins.root"
    outputFileName = "WR_NR_MuMuJJ_ResolvedSignalRegion.root"

else:
    fileName = "WR_NR_MuMuJJ_BoostedSignalRegion_400GeVBins.root"
    #fileName = "WR_NR_MuMuJJ_BoostedSignalRegion_400BINS.root"
    outputFileName = "WR_NR_MuMuJJ_BoostedSignalRegion.root"

output = "SoverB"
signal = "WR_5000_NR_200"
folder = "MuMuJJ/"

c = ROOT.TCanvas("c","c",1000,1000)
c.Divide(2,4)
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
    	    else:
            	fileA.Get(folder+hist.GetName()).Write()


#MAIN BGS
DYhist = fileA.Get(folder+"DY")
#DYhist = SetEmptyBinErrorToOne(DYhist)
EmuHist = fileA.Get(folder+"EMu")
SThist = fileA.Get(folder+"ST")
DiBosonHist = fileA.Get(folder+"DiBoson")
TriBosonHist = fileA.Get(folder+"TriBoson")
ttVhist = fileA.Get(folder+"ttV")
WJetsHist = fileA.Get(folder+"WJets")

#DY SYSTEMATICS
if isResolved == "True":
    DYhist_JECUp = fileA.Get(folder+"DY_JECUp")
    DYhist_JECDown = fileA.Get(folder+"DY_JECDown")
    DYhist_JERUp = fileA.Get(folder+"DY_JERUp")
    DYhist_JERDown = fileA.Get(folder+"DY_JERDown")
    DYhist_PUUp = fileA.Get(folder+"DY_PUUp")
    DYhist_PUDown = fileA.Get(folder+"DY_PUDown")
    DYhist_MuIDUp = fileA.Get(folder+"DY_MuResIDUp")
    DYhist_MuIDDown = fileA.Get(folder+"DY_MuResIDDown")
    DYhist_MuResolUp = fileA.Get(folder+"DY_MuResolUp")
    DYhist_MuResolDown = fileA.Get(folder+"DY_MuResolDown")
    DYhist_MuTrigUp = fileA.Get(folder+"DY_MuTrigUp")
    DYhist_MuTrigDown = fileA.Get(folder+"DY_MuTrigDown")

else:
    DYhist_JECUp = fileA.Get(folder+"DY_JECUp")
    DYhist_JECDown = fileA.Get(folder+"DY_JECDown")
    DYhist_JERUp = fileA.Get(folder+"DY_JERUp")
    DYhist_JERDown = fileA.Get(folder+"DY_JERDown")
    DYhist_PUUp = fileA.Get(folder+"DY_PUUp")
    DYhist_PUDown = fileA.Get(folder+"DY_PUDown")
    DYhist_MuIDUp = fileA.Get(folder+"DY_MuIDUp")
    DYhist_MuIDDown = fileA.Get(folder+"DY_MuIDDown")
    DYhist_MuResolUp = fileA.Get(folder+"DY_MuResolUp")
    DYhist_MuResolDown = fileA.Get(folder+"DY_MuResolDown")
    DYhist_MuTrigUp = fileA.Get(folder+"DY_MuTrigUp")
    DYhist_MuTrigDown = fileA.Get(folder+"DY_MuTrigDown")
    DYhist_LSFUp = fileA.Get(folder+"DY_LSFUp")
    DYhist_LSFDown = fileA.Get(folder+"DY_LSFDown")

#bigBinEmu = EmuHist.Rebin(7, "bigBinEmu")
EmuHist = removeNegBins(EmuHist)
#OTHER BGS
otherBGhist = histFromStack(myOtherStack)
newBGstack = ROOT.THStack() #DY, EMU, OTHER
#otherBGhist = otherBGhist.Rebin(5, "",binning)
#bigBinBGhist = otherBGhist.Rebin(5, "",binning)
#bigBinDYhist = DYhist.Rebin(5, "", binning)
#bigBinEmuHist = EmuHist.Rebin(5, "", binning)
#bigBinSTHist = SThist.Rebin(5, "", binning)
#bigBinDiBosonHist = DiBosonHist.Rebin(5, "", binning)
#bigBinTriBosonHist = TriBosonHist.Rebin(5, "", binning)
#bigBinttVhist = ttVhist.Rebin(5, "", binning)
#bigBinWJetsHist = WJetsHist.Rebin(5, "", binning)

#TEMPLATES FOR LATER
DYtemplate  = copy.deepcopy(otherBGhist)
EMUtemplate = copy.deepcopy(otherBGhist)
DYtemplate.Scale( 0.0)
EMUtemplate.Scale(0.0)

#MAIN STUFF
histoB = histFromStack(myStackA)
histoA = fileA.Get(folder+signal)

histoA.SetTitle("")
histoB.SetTitle("")

histoA.Sumw2()
histoB.Sumw2()

nBinsA = histoA.GetNbinsX()
nBinsB = histoB.GetNbinsX()

print "NBINS:"
print nBinsA
print nBinsB

integralA = 0.0
integralB = 0.0

if (nBinsA != nBinsB):
    print "incompatible histogram binning"
    exit(0)

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
integralA = histoA.Integral()
integralB = histoB.Integral()
print histoA.Integral()
print histoB.Integral()

if (integralA == 0.0 or integralB == 0.0):
    print "histogram with 0 for integral... cannot compute"
    exit(0)

#RATIO HISTO
ratioHisto = copy.deepcopy(histoA)
ratioHisto.Scale(0.0)
ratioHisto.Sumw2()


ratioHisto = copy.deepcopy(histoA)
ratioHisto.Scale(0.0)
ratioHisto.Sumw2()

for ibin in range(1,nBinsA+1):  
    if(histoB.GetBinContent(ibin) == 0):
        newBinVal = 0.0
        newBinErr = 0.0
        continue
    newBinVal = histoA.GetBinContent(ibin) / histoB.GetBinContent(ibin)
    newBinErr = ( ( histoA.GetBinError(ibin) * ( 1 / histoB.GetBinContent(ibin) ) )**2.0 + ( histoA.GetBinContent(ibin) * ( histoB.GetBinError(ibin) / ( histoB.GetBinContent(ibin) )**2.0 ) )**2.0 ) ** 0.5

    print "BIN"+str(newBinVal)

    ratioHisto.SetBinContent(ibin, newBinVal)
    ratioHisto.SetBinError(ibin, newBinErr)



#DRAW TIME
maxA = histoA.GetMaximum()
maxB = histoB.GetMaximum()
maxR = ratioHisto.GetMaximum()
maxes = [maxA, maxB]
maxes.sort()
newMax = maxes[-1]*5.0

minA = histoA.GetMinimum()
minB = histoB.GetMinimum()
print "MINVALB"
print histoB.GetMinimumBin()
print histoB.GetBinCenter(histoB.GetMinimumBin())
minR = ratioHisto.GetMinimum()
print "MINS"
mins = [minA, minB]
print mins
mins.sort()
newMin = mins[0]*0.1
if newMin <= 0.0:
    newMin = 0.00000001

c.cd(1)
ROOT.gPad.SetLogy()
histoA.SetMaximum(newMax)
histoA.SetMinimum(newMin)
histoA.SetLineColor(ROOT.kBlack)
histoA.Draw("e")

histoB.SetMaximum(newMax)
histoB.SetMinimum(newMin)
histoB.SetLineColor(ROOT.kRed)
histoB.Draw("esame")

myLeg = ROOT.TLegend( 0.2 , 0.1 , 0.6 , 0.3 )

bigBin = histoB.Rebin(nBinsDY, "", binningDY)
bigBin.SetLineColor(ROOT.kBlue)
bigBin.Draw("esame")

myLeg.AddEntry(histoB, "Background Stack   ")
myLeg.AddEntry(histoA, "M_{WR} 5000 M_{NR} 200 Signal   ")
myLeg.SetMargin(0.4)
myLeg.Draw()

histoA.GetXaxis().SetRangeUser(0.0,8000)
histoB.GetXaxis().SetRangeUser(0.0,8000)

#FIT FORMS
expForm = 'exp([0]-[1]*x)'
expXpow = '[0]*exp(-[1]*x)/(pow(x,[2]))'
expXpow1 = '[0]*exp(-[1]*x)/(pow(x,1.0))'
expXpow2 = '[0]*exp(-[1]*x)/(pow(x,2.0))'
expXpow3 = '[0]*exp(-[1]*x)/(pow(x,3.0))'
expXpow4 = '[0]*exp(-[1]*x)/(pow(x,4.0))'
expXpow5 = '[0]*exp(-[1]*x)/(pow(x,5.0))'
expXpow6 = '[0]*exp(-[1]*x)/(pow(x,6.0))'
expXpow7 = '[0]*exp(-[1]*x)/(pow(x,7.0))'
expXpow8 = '[0]*exp(-[1]*x)/(pow(x,8.0))'
#pows    = '10.0*pow((1 - x) , 2) * pow( (1 + x) , ([0] * x) )'
pows    = '[0]* pow( (1 + x) , ([1] * x) )'
expPow  = '[0]*(1 - [1]*x + pow([1]*x,2.0)/2.0 - pow([1]*x,3.0)/6.0 + pow([1]*x,4.0)/24.0 - pow([1]*x,5.0)/120.0 + pow([1]*x,6)/720.0 - pow([1]*x,7)/5040.0 + pow([1]*x,8)/40320.0 - pow([1]*x,9)/362880.0 + pow([1]*x,10)/3628800.0 - pow([1]*x,11)/39916800.0 + pow([1]*x,12)/479001600.0)'
#expPow  = '[1]*(1 - [0]*x)'
lnexpXpow = '[0]*(x - [1]*log(x))+[2]'
#expMod  = '[0]*exp(-((x-800.0)/([1]))^[2])'
expMod  = '[0]*exp(-(x-800.0)/([1]))'
expXpow  = '[0]*exp(-((x-800.0)^[2])/([1]))'

atlasFunc = '[0]*pow((1-(x/13000.0)),[1])*pow((1+(x/13000.0)),([2]*(x/13000.0)))'

#fitFormDY  = atlasFunc
fitFormDY  = expForm
#fitFormDY  = expXpow
fitFormDYbig = fitFormDY
fitFormEMU = expXpow4

#DY ONLY########################################################################
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

if not (isResolved == "True"):
    bigBinDY = DYhist.Rebin(nBinsDY, "", binningDY)
    bigBinDY = histBinSplit(bigBinDY, 13)


fitFunctionDY = ROOT.TF1('funcDY', fitFormDY, 800.0, 8000.0)
#fitFunctionDY.SetParameters(10.0, 500.0)
#fitFunctionDY.SetParameters(10.0,500.0,0.5)
#fitFunctionDY.SetParNames("Power1", "Power2")
#fitFunctionDY.SetParLimits(0, 2.5, 250.0)
#fitFunctionDY.SetParLimits(1, 500.0, 800.0)
#fitFunctionDY.SetParLimits(1, 25.0, 2500.0)
#fitFunctionDY.SetParLimits(2, 0.1, 3.0)


#fitOptions = "IEMVRS"
#fitOptions = "IWLEMVRS"
#fitOptions = "IWLEVRS"
fitOptions = "IWLVRS"
#fitOptions = "IWLMVRS"

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

lowEdgeHist = getHistoLow(DYtemplate)
highEdgeHist = getHistoUp(DYtemplate)

lowEdgeHist.Draw("Csame")
highEdgeHist.Draw("Csame")

#EMU ONLY#########################################################
c.cd(4)
print ""
print "Emu MINIMUM:"
#EmuHist = bigBinEmu
print EmuHist.GetMinimum()
print EmuHist.GetBinCenter(EmuHist.GetMinimumBin())

ROOT.gPad.SetLogy()
#EmuHist = logHist(EmuHist)
EmuHist.SetMinimum(0.000000001)
EmuHist.SetLineColor(ROOT.kGreen)
EmuHist.SetTitle("EMU")
EmuHist.SetLineWidth(3)
#EmuHist.Draw("e")

bigBinEMU = EmuHist.Rebin(nBinsEMU, "", binningEMU)
if isResolved == "True":
    bigBinEMU = histBinSplit(bigBinEMU, 14)
else:
    bigBinEMU = histBinSplit(bigBinEMU, 15)

bigBinEMU.SetLineColor(ROOT.kRed)

fitFunctionEMU = ROOT.TF1('funcEMU', expMod, 800, 8000)

fitFunctionEMU.SetParameters(10.0,500.0)
#fitFunctionDY.SetParNames("Power1", "Power2")
#fitFunctionEMU.SetParLimits(0, 2.5, 250.0)
#fitFunctionDY.SetParLimits(1, 500.0, 800.0)
#fitFunctionEMU.SetParLimits(1, 25.0, 2500.0)
#fitFunctionEMU.SetParLimits(2, 0.1, 3.0)


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

#SAVING DY AND EMU################################################
#DYtemplate.SetName("DY")
#EMUtemplate.SetName("EMu")
#DYtemplate.Write()
#EMUtemplate.Write()

#RATIO PLOT############################################################
c.cd(3)
ROOT.gPad.SetLogy()
ratioHisto.SetLineColor(ROOT.kBlack)
ratioHisto.SetTitle("S/B")
ratioHisto.Draw("e")

#c.cd(2)
#myLeg = ROOT.TLegend( .6 , .5 , 1.0 , 1.0 )
#
#myLeg.AddEntry(histoB, "Background Stack   ")
#myLeg.AddEntry(histoA, "M_{WR} 5000 M_{NR} 200 Signal   ")
#myLeg.AddEntry(EmuHist, "EMu")
#myLeg.AddEntry(DYhist, "Drell-Yan")
#myLeg.SetMargin(0.4)
#myLeg.Draw()

#CONSTRUCTING THE TOTAL PICTURE#######################################

#ADDING UNCERTAINTIES
if isResolved == "True":
    bigBinDY = DYhist   #FOR RESOLVED
addUncertainty(DYtemplate, bigBinDY)
addUncertainty(EMUtemplate, bigBinEMU)

#HISTS MADE TIME TO STACK
DYtemplate.SetLineColor(ROOT.kBlue)
newBGstack.Add(DYtemplate)
EMUtemplate.SetLineColor(ROOT.kGreen)
newBGstack.Add(EMUtemplate)
otherBGhist.SetLineColor(ROOT.kBlack)
newBGstack.Add(otherBGhist)

#COMBINED HIST
newBGhist = histFromStack(newBGstack)
addUncertainty(DYtemplate,bigBinDY)
addUncertainty(EMUtemplate,bigBinEMU)
oldBGhist = histoB

####CREATING DY SYSTEMATIC HISTOGRAMS#############################
if isResolved == "True":
    bigBinDY_temp = DYhist.Rebin(6, "", binningDY)
    DYhist_JECUp_temp = DYhist_JECUp.Rebin(6, "", binningDY)
    DYhist_JECDown_temp = DYhist_JECDown.Rebin(6, "", binningDY)
    DYhist_JERUp_temp = DYhist_JERUp.Rebin(6, "", binningDY)
    DYhist_JERDown_temp = DYhist_JERDown.Rebin(6, "", binningDY)
    DYhist_PUUp_temp = DYhist_PUUp.Rebin(6, "", binningDY)
    DYhist_PUDown_temp = DYhist_PUDown.Rebin(6, "", binningDY)
    DYhist_MuIDUp_temp = DYhist_MuIDUp.Rebin(6, "", binningDY)
    DYhist_MuIDDown_temp = DYhist_MuIDDown.Rebin(6, "", binningDY)
    DYhist_MuResolUp_temp = DYhist_MuResolUp.Rebin(6, "", binningDY)
    DYhist_MuResolDown_temp = DYhist_MuResolDown.Rebin(6, "", binningDY)
    DYhist_MuTrigUp_temp = DYhist_MuTrigUp.Rebin(6, "", binningDY)
    DYhist_MuTrigDown_temp = DYhist_MuTrigDown.Rebin(6, "", binningDY)

else:
    bigBinDY_temp = DYhist.Rebin(6, "", binningDY)
    DYhist_JECUp_temp = DYhist_JECUp.Rebin(6, "", binningDY)
    DYhist_JECDown_temp = DYhist_JECDown.Rebin(6, "", binningDY)
    DYhist_JERUp_temp = DYhist_JERUp.Rebin(6, "", binningDY)
    DYhist_JERDown_temp = DYhist_JERDown.Rebin(6, "", binningDY)
    DYhist_PUUp_temp = DYhist_PUUp.Rebin(6, "", binningDY)
    DYhist_PUDown_temp = DYhist_PUDown.Rebin(6, "", binningDY)
    DYhist_MuIDUp_temp = DYhist_MuIDUp.Rebin(6, "", binningDY)
    DYhist_MuIDDown_temp = DYhist_MuIDDown.Rebin(6, "", binningDY)
    DYhist_MuResolUp_temp = DYhist_MuResolUp.Rebin(6, "", binningDY)
    DYhist_MuResolDown_temp = DYhist_MuResolDown.Rebin(6, "", binningDY)
    DYhist_MuTrigUp_temp = DYhist_MuTrigUp.Rebin(6, "", binningDY)
    DYhist_MuTrigDown_temp = DYhist_MuTrigDown.Rebin(6, "", binningDY)
    DYhist_LSFUp_temp = DYhist_LSFUp.Rebin(6, "", binningDY)
    DYhist_LSFDown_temp = DYhist_LSFDown.Rebin(6, "", binningDY)

DY_JECUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_JECUp_temp)
DY_JECDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_JECDown_temp)
DY_JERUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_JERUp_temp)
DY_JERDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_JERDown_temp)
DY_PUUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_PUUp_temp)
DY_PUDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_PUDown_temp)
DY_MuResolUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuResolUp_temp)
DY_MuResolDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuResolDown_temp)
DY_MuTrigUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuTrigUp_temp)
DY_MuTrigDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuTrigDown_temp)
if isResolved == "True":
    DY_MuIDUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuIDUp_temp)
    DY_MuIDDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuIDDown_temp)

else:
    DY_MuIDUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuIDUp_temp)
    DY_MuIDDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_MuIDDown_temp)
    DY_LSFUpTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_LSFUp_temp)
    DY_LSFDownTemplate = createSystematicTemplate(DYtemplate, bigBinDY_temp, DYhist_LSFDown_temp)

DY_JECUpTemplate.SetName("DY_JECUp")
DY_JECDownTemplate.SetName("DY_JECDown")
DY_JERUpTemplate.SetName("DY_JERUp")
DY_JERDownTemplate.SetName("DY_JERDown")
DY_PUUpTemplate.SetName("DY_PUUp")
DY_PUDownTemplate.SetName("DY_PUDown")
DY_MuResolUpTemplate.SetName("DY_MuResolUp")
DY_MuResolDownTemplate.SetName("DY_MuResolDown")
DY_MuTrigUpTemplate.SetName("DY_MuTrigUp")
DY_MuTrigDownTemplate.SetName("DY_MuTrigDown")
if isResolved == "True":
    DY_MuIDUpTemplate.SetName("DY_MuResIDUp")
    DY_MuIDDownTemplate.SetName("DY_MuResIDDown")
else:
    DY_MuIDUpTemplate.SetName("DY_MuIDUp")
    DY_MuIDDownTemplate.SetName("DY_MuIDDown")
    DY_LSFUpTemplate.SetName("DY_LSFUp")
    DY_LSFDownTemplate.SetName("DY_LSFDown")

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
if not (isResolved == "True"):
    DY_LSFUpTemplate.Write()
    DY_LSFDownTemplate.Write()

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

newBGhist.SetTitle("")
oldBGhist.SetTitle("")

c.cd(6)
ROOT.gPad.SetLogy()
newBGhist.SetMaximum(500.0)
newBGhist.SetMinimum(0.00000000001)
newBGhist.SetLineColor(ROOT.kBlue)
newBGhist.Draw("e")
#newBGstack.Draw()
oldBGhist.SetLineColor(ROOT.kRed)
#oldBGhist.Draw("esame")
EMUtemplate.SetTitle("")
EMUtemplate.Draw("e")
DYtemplate.Draw("esame")
otherBGhist.Draw("esame")

newLeg = ROOT.TLegend(0.7,0.7,1.0,1.0)
newLeg.AddEntry(EMUtemplate, "EMU FIT")
newLeg.AddEntry(DYtemplate, "DY FIT")
newLeg.AddEntry(otherBGhist, "Other BGs")
newLeg.Draw()
c.cd(8)
ROOT.gPad.SetLogy()
newBGhist.SetTitle("Total BGs post fit")
newBGhist.Draw()
#PROCEDURE RATIO
BGratioHisto = copy.deepcopy(newBGhist)
BGratioHisto.Scale(0.0)

for ibin in range(1,nBinsA+1):  
    if(oldBGhist.GetBinContent(ibin) == 0):
        newBinVal = 0.0
        newBinErr = 0.0
        continue
    newBinVal = newBGhist.GetBinContent(ibin) / oldBGhist.GetBinContent(ibin)
    newBinErr = ( ( newBGhist.GetBinError(ibin) * ( 1 / oldBGhist.GetBinContent(ibin) ) )**2.0 + ( newBGhist.GetBinContent(ibin) * ( oldBGhist.GetBinError(ibin) / ( oldBGhist.GetBinContent(ibin) )**2.0 ) )**2.0 ) ** 0.5

    print "BIN"+str(newBinVal)

    BGratioHisto.SetBinContent(ibin, newBinVal)
    BGratioHisto.SetBinError(ibin, newBinErr)

newBGratioHisto = copy.deepcopy(newBGhist)
newBGratioHisto.Scale(0.0)

for ibin in range(1,nBinsA+1):  
    if(newBGhist.GetBinContent(ibin) == 0):
        newBinVal = 0.0
        newBinErr = 0.0
        continue
    newBinVal = histoA.GetBinContent(ibin) / newBGhist.GetBinContent(ibin)
    newBinErr = ( ( histoA.GetBinError(ibin) * ( 1 / newBGhist.GetBinContent(ibin) ) )**2.0 + ( histoA.GetBinContent(ibin) * ( newBGhist.GetBinError(ibin) / ( newBGhist.GetBinContent(ibin) )**2.0 ) )**2.0 ) ** 0.5

    print "BIN"+str(newBinVal)

    newBGratioHisto.SetBinContent(ibin, newBinVal)
    newBGratioHisto.SetBinError(ibin, newBinErr)

c.cd(3)
newBGratioHisto.Draw("esame")

c.cd(5)
ROOT.gPad.SetLogy()
BGratioHisto.SetTitle("original / fit")
BGratioHisto.Draw()


c.SaveAs(output+"_ratioPlot.png")
ratioHisto.SaveAs(output+"_ratioAlone.root")
ratioHisto.SaveAs(output+"_ratioAlone.png")


