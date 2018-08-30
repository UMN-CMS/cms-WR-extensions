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

import numpy as np
def getRatio(hist, reference):
        ratio = hist.Clone("%s_ratio"%hist.GetName())
        ratio.SetDirectory(0)
        ratio.SetLineColor(hist.GetLineColor())
        for xbin in xrange(1,reference.GetNbinsX()+1):
                ref = reference.GetBinContent(xbin)
                val = hist.GetBinContent(xbin)

                refE = reference.GetBinError(xbin)
                valE = hist.GetBinError(xbin)

                try:
                        ratio.SetBinContent(xbin, val/ref)
                        ratio.SetBinError(xbin, math.sqrt( (val*refE/(ref**2))**2 + (valE/ref)**2 ))
                except ZeroDivisionError:
                        #ratio.SetBinContent(xbin, 1.0)
                        ratio.SetBinError(xbin, 0.0)

        return ratio

def TOTerror(hmc, ratio ):
  hmc.Sumw2()
  den1 = hmc.Clone ("den1");
  den2 = hmc.Clone ("den2");

  nvar = hmc.GetNbinsX();

  x1 = []
  y1 = []
  exl1 = []
  eyl1= []
  exh1= []
  eyh1= []

  for km in range(1,nvar+1):
    delta = hmc.GetBinError(km)
    den1.SetBinError(km,0)

  # ratio from variation and nominal
  ratiop = hmc.Clone("ratiop");
  ratiom = hmc.Clone("ratiom");

  ratiop.Divide(den1);
  ratiom.Divide(den1);
  #den1.Divide(ratiop)
  #den2.Divide(ratiom)

  return ratiop;

def customPalette(zeropoint = 0.5):
    Number = 3;
    Red    = np.array([0  ,  100,  110],dtype=float)/255.
    Green  = np.array([0  ,  255,  0], dtype=float)/255.
    Blue   = np.array([99 ,  100,  2], dtype=float)/255.
    Length = np.array([0.0,  zeropoint, 1.0], dtype=float)
    nb=100;
    ROOT.TColor.CreateGradientColorTable(Number,Length,Red,Green,Blue,nb)


def saveHistsTDir(file,bgStacksRootDir,outPath,directory="",prefix="",bg="simple", eventsWeight=1.0, dataType = "MC", setLog=0):
    hists1d = ["TH1D", "TH1F", "TH1"]
    hists2d = ["TH2D", "TH2F", "TH2"]
    histObjectNames = hists1d + hists2d
    for key in file.GetListOfKeys():
        print "LOOPING THROUGH:"
        print key.GetName()
        sys.stdout.flush()
        if key.IsFolder():
            dir = file.Get(key.GetName())
            newDir=directory+"/"+key.GetName()
            if(not (os.path.isdir(newDir))):
                subprocess.call(["mkdir", newDir])
            saveHistsTDir(file,bgStacksRootDir,newDir,directory=newDir, prefix=prefix,bg=bg, eventsWeight=eventsWeight, dataType=dataType, setLog=setLog)
        if key.GetClassName() in histObjectNames and filter in prefix:
            hist = file.Get(key.GetName())
            drawoptions = ""
            if key.GetClassName() in hists2d:
                drawoptions = "colz"
        print "drawHist"
        drawHist(hist,bgStacksRootDir,directory,newDir,prefix,key.GetName(),".png",width=1000,height=800, drawoptions = "", bg=bg, eventsWeight=eventsWeight, dataType=dataType, setLogy=setLog)


def saveHistsTStack(folder,bgStacksRootDir,outPath,directory="",prefix="",bg="simple", eventsWeight=1.0, dataType = "MC", setLog=0):

    for entry in os.listdir(folder):
        print "LOOPING THROUGH:"
        print entry
        absPathObj = folder+"/"+entry
        sys.stdout.flush()
        newOutPathFolder = outPath
        if os.path.isdir(absPathObj):
            newOutPathFolder = outPath+"/"+entry
            newDir=directory+"/"+entry
            if(not (os.path.isdir(newOutPathFolder))):
                subprocess.call(["mkdir", newOutPathFolder])
            saveHistsTStack(absPathObj,bgStacksRootDir,newOutPathFolder,directory=newDir, prefix=prefix,bg=bg, eventsWeight=eventsWeight, dataType=dataType, setLog=setLog)
        myHisto = 0
        if os.path.isfile(absPathObj) and ".root" in entry:
            histoROOTfile = ROOT.TFile(absPathObj,"READ")
            for key in histoROOTfile.GetListOfKeys():
                print key.GetName()
                print key.GetClassName()
                if ("THStack" in key.GetClassName()):
                    myHisto = histoROOTfile.Get(key.GetName())
                    break
            if (myHisto == 0):
                print "BAD ROOT FILE, SKIPPING..."
                continue 
	    print "Drawing Hist"
            print myHisto
            #myHisto.Draw()
            combiHisto = histFromStackNoRef(myHisto)
            drawHist(combiHisto,bgStacksRootDir,directory,newOutPathFolder,prefix,entry[:-5],".png",width=1000,height=800, drawoptions = "", bg=bg, eventsWeight=eventsWeight, dataType=dataType, setLogy=setLog)

def getStack(folder, plotName):
    print folder+"/"+plotName+".root"
    sys.stdout.flush()
    if not os.path.isfile(folder+"/"+plotName+".root"):
        return 0
    file = ROOT.TFile.Open(folder+"/"+plotName+".root", "read")
    for key in file.GetListOfKeys():
        if (not key.IsFolder()) and (key.GetName() == plotName):
            print "THSTACK PLOT FOUND"
            sys.stdout.flush()
            return file.Get(key.GetName())
    return 0
def histFromStackWithRef(refHisto, stack): #CREATES A HISTOGRAM FROM A THSTACK
    newHist = copy.deepcopy(refHisto)
    newHist.Scale(0.0)
    newHist.Merge(stack.GetHists())
    return newHist
def histFromStackNoRef(stack):
    newHist = copy.deepcopy(stack.GetHists()[0])
    newHist.Scale(0.0)
    newHist.Merge(stack.GetHists())
    return newHist
    
def drawHist(hist,bgStacks,stackPlotRelPath,outpathDir,prefix,name,postfix,width=1500,height=1500, drawoptions="",bg="simple", eventsWeight=1.0, dataType = "MC", setLogy = 0):
#/home/aevans/public_html/plots/21_Aug_2017_14-49-11-CDT//demo/eventsPassingWR2016RECO/WR_M-4000_ToLNu_M-1333_Analysis_MuMuJJ_selectedJetEta.png
    weight = 1.0
    if (dataType == "MC") :
        print "PLOTTING MC: SCALING TO 2016 DATA"
        weight *= 35900.0
        weight *= 1.0 #pb just for plotting purposes
       # weight *= 0.9820342339793464  #FUDGE FACTOR BASED ON ROOT FILES NEEDING RESKIMMED FOR ELECTRON FLAVOR SIDEBAND (127 OUT OF 7069)
       # weight *= 0.65                #FUDGE FACTOR FOR MUON DATA BASED ON Z PEAK SIDEBAND DISAGREEMENT
       # weight *= 0.6641282341721065  #FUDGE FACTOR CAUSE I'M MISSING EVENTS FOR THE MUON DATA 5.339658e8 / 804010088
    weight /= eventsWeight
                               
    newHist = copy.deepcopy(hist)
    print hist
    print newHist
    print hist.GetYaxis()
    print newHist.GetYaxis()
    newHist.GetYaxis().SetTitle("Events")
    c = ROOT.TCanvas("c","c",width,height)

    oben = ROOT.TPad('oben','oben',0,0.3 ,1.0,1.0)
    unten = ROOT.TPad('unten','unten',0,0.0,1.0,0.3)
    oben.SetBottomMargin(0)
    unten.SetTopMargin(0.)
    unten.SetBottomMargin(0.35)

    oben.SetFillStyle(4000)
    oben.SetFrameFillStyle(1000)
    oben.SetFrameFillColor(0)
    unten.SetFillStyle(4000)
    unten.SetFrameFillStyle(1000)
    unten.SetFrameFillColor(0)
    oben.Draw()
    unten.Draw()
    oben.cd()

    #hist.SetLineWidth(2)
    #print bg
    if (bg == "backgrounds"):
#	print "name.split: ", name.split("/")[-1].split("_")[-1][:-3]
        backgroundStack = getStack(bgStacks+stackPlotRelPath,name)
        if (backgroundStack != 0):
            print "GOT THE BACKGROUND"    
            sys.stdout.flush()
        else:
            print "NO BACKGROUNDS FOUND!"
            sys.stdout.flush()
            return
    if not (newHist.GetMaximum() == 0) : 
        print "SCALING HISTOGRAM"
        sys.stdout.flush()
        backgroundCombined = copy.deepcopy(newHist)
        backgroundCombined.Scale(0.0)
        backgroundCombined.Merge(backgroundStack.GetHists())
        
        scaleFactor = weight
	print "scaleFactor: "
        print scaleFactor
        sys.stdout.flush()
        if scaleFactor == 0.0 :
            print "NOT USING SILLY SCALE FACTOR"
            sys.stdout.flush()
            scaleFactor = 1.0
        newHist.Scale(scaleFactor)
        newHist.SetLineColor(ROOT.kBlack)
        newHist.SetLineStyle(1)
        newHist.SetLineWidth(2)
	newHist.SetFillStyle(0)
	newHist.GetYaxis().SetTitleOffset(1.3)
        newHist.GetYaxis().SetLabelSize(.05)
        newHist.GetYaxis().SetTitleSize(.05)
	newHist.GetXaxis().SetLabelOffset(100)

        if (newHist.GetMaximum() > backgroundCombined.GetMaximum()) :
            newHist.SetMaximum(newHist.GetMaximum()*1.3)
            backgroundStack.SetMaximum(newHist.GetMaximum()*1.3)
        else :
            newHist.SetMaximum(backgroundCombined.GetMaximum()*1.3)
            backgroundStack.SetMaximum(backgroundCombined.GetMaximum()*1.3)
        backgroundStack.SetTitle("")
        newHist.SetTitle("")
	if setLogy:
	    newHist.SetMinimum(1)
	    if (newHist.GetMaximum() > backgroundCombined.GetMaximum()) :
	      newHist.SetMaximum(newHist.GetMaximum()*4)
              backgroundStack.SetMaximum(newHist.GetMaximum()*4)
            else :
              newHist.SetMaximum(backgroundCombined.GetMaximum()*4)
              backgroundStack.SetMaximum(backgroundCombined.GetMaximum()*4)

#        backgroundStack.Draw("HIST")
        if(dataType == "MC"):
            newHist.Draw(drawoptions)
        else:
            newHist.Draw("e")
        backgroundStack.Draw("HIST same")
	newHist.Draw("esame")
	print "Data Integral: ", newHist.Integral()
	print "MC Integral: ", backgroundStack.GetStack().Last().Integral()
#        if(dataType == "MC"):
#            newHist.Draw(drawoptions+"same")
#        else:
#            newHist.Draw("esame")
        allMC=backgroundStack.GetStack().Last().Clone()

	herr = allMC.Clone('herr')

        theErrorGraph = ROOT.TGraphErrors(herr)
        theErrorGraph.SetFillColor(ROOT.kGray+2)
        theErrorGraph.SetFillStyle(3002)
        herr.SetFillColor(ROOT.kGray+2)
        herr.SetFillStyle(3002)
        herr.SetMarkerColor(1111);

        theErrorGraph.Draw('SAME2')

        print "Accessing X Axis Titles"
        print backgroundStack.GetXaxis().GetTitle()
        print newHist.GetXaxis().GetTitle()
        newHistTitle = newHist.GetXaxis().GetTitle()
        sys.stdout.flush()
        global backgroundPlotNames
	print "backgroundPlotNames: ", backgroundPlotNames
        leg_y = 0.88 - (2+int(len(backgroundPlotNames)/2))*0.03
        legend = ROOT.TLegend(0.19,leg_y,0.42,0.88)
	legend.SetFillStyle(0)
        legend.SetBorderSize(0)
        legend.SetTextSize(0.035)
        legend.SetTextFont(42)
        legend2 = ROOT.TLegend(0.42,leg_y,0.65,0.88)
        legend2.SetFillStyle(0)
        legend2.SetBorderSize(0)
        legend2.SetTextSize(0.035)
        legend2.SetTextFont(42)
        legend3 = ROOT.TLegend(0.65,leg_y,0.88,0.88)
        legend3.SetFillStyle(0)
        legend3.SetBorderSize(0)
        legend3.SetTextSize(0.035)
        legend3.SetTextFont(42)
        massPoint = ["NA", "NA"]
        if(dataType == "MC"):
            legend3.AddEntry(newHist, "Signal MC M_WR "+str(massPoint[0])+" M_NR "+str(massPoint[1]))
        else:
            legend3.AddEntry(newHist, "2016 Data", "L")
	print "backgrounds: ", backgrounds

	count=1
        for bg in backgrounds:
	    print "bg: ", bg
	    print "backgroundPlotNames[bg]: ", backgroundPlotNames[bg]
            if count < 4: legend.AddEntry(backgroundPlotNames[bg], bg, "F")
	    elif count > 3: legend2.AddEntry(backgroundPlotNames[bg], bg, "F")
	    count = count+1

	legend3.AddEntry(herr,"MC uncert. (stat.)","fl")

	legend.Draw()
	legend2.Draw()
	legend3.Draw()
        #Now build the legend
        if setLogy == 1:
	    print "SETTING LOGy"
	    oben.SetLogy()
	print "name: ", name

	lumi = 35.9
        tag1 = ROOT.TLatex(0.72,0.92,"%.1f fb^{-1} (13 TeV)"%lumi)
        tag1.SetNDC(); tag1.SetTextFont(42)
        tag1.SetTextSize(0.045)
        tag2 = ROOT.TLatex(0.17,0.92,"CMS")
        tag2.SetNDC()
        tag2.SetTextFont(62)
        tag3 = ROOT.TLatex(0.24,0.92,"Preliminary")
        tag3.SetNDC()
        tag3.SetTextFont(52)
        tag2.SetTextSize(0.055)
        tag3.SetTextSize(0.045)
        tag1.Draw()
        tag2.Draw()
        tag3.Draw()

	unten.cd()
        ratio = getRatio(newHist,allMC)
        herr3= TOTerror(allMC,ratio);
        toterree = ROOT.TGraphErrors(herr3)

        ratio.SetStats(0)
        ratio.GetYaxis().SetRangeUser(0,5)
        ratio.GetYaxis().SetNdivisions(504)
        ratio.GetYaxis().SetTitle("Data/Simulation")
        ratio.GetXaxis().SetTitle(allMC.GetXaxis().GetTitle())
        ratio.GetXaxis().SetTitleSize(0.14)
        ratio.GetXaxis().SetTitleOffset(1.0)
        ratio.GetXaxis().SetLabelOffset(0.005)
        ratio.GetYaxis().SetTitleOffset(0.5)
        ratio.GetYaxis().SetLabelSize(0.12)
        ratio.GetYaxis().SetTitleSize(0.11)
        ratio.GetXaxis().SetLabelSize(0.11)

        line = ROOT.TLine(ratio.GetXaxis().GetXmin(), 1.0,
                      ratio.GetXaxis().GetXmax(), 1.0)
        line.SetLineColor(ROOT.kGray)
        line.SetLineStyle(2)
        line.Draw()
        tKsChi = ROOT.TLatex()
        tKsChi.SetNDC()
        tKsChi.SetTextFont(42)
        tKsChi.SetTextSize(0.09)

        ratio.Draw("P E ")

        toterree.SetFillColor(ROOT.kGray+2)
        toterree.SetLineColor(ROOT.kGray+2)
        toterree.SetFillStyle(3002)
#        toterree.Draw("2 same")
        line.Draw("same")

        leg4 = ROOT.TLegend(0.7,0.89,0.5,0.8)
        leg4.SetFillStyle(0)
        leg4.SetBorderSize(0)
        leg4.SetTextSize(0.05)
        leg4.SetTextFont(42)
        leg4.AddEntry(toterree,"MC uncert. (stat.)","fl")
        leg4.Draw()

        c.SaveAs(outpathDir+"/"+name+postfix)
    else:
        print "HISTOGRAM EMPTY!"
        sys.stdout.flush()

#############################################################################################
#WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root
#SingleMuon_WR_M-2400_LNu_M-240.root
print "STARTING"
if len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "h"):
    print "=========="
    print "This program takes collections histogram stacks ROOT file by ROOT file and \npairs and plots  each set together into one ROOT file"
    print "In order, please specify:"
    print "Location of primary stack directory (data/signal)"
    print "Directory of where to make plots"
    print "Text file contain list of datasets to compare to:"
    print "Directory where the comparison datasets are stored:"
    print "DATA or MC plots"
    print "Optional prefix for output naming"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python superPlotter2000.py SingleElectron 1 /afs/cern.ch/work/a/aevans/public/thesis/dataStacks/ ../../../plots/ /afs/cern.ch/work/a/aevans/public/thesis/backgroundStacks/ ../../../samples/backgrounds/fullBackgroundDatasetList_no_ext_noDiBoson.txt /afs/cern.ch/work/a/aevans/public/backgroundStacks/ DATA prefix"
    print ""
    exit(0)
if len(sys.argv) != 10:
    print "inputs not understood, try help/h"
    exit(1)
sys.stdout.flush()

signalName = sys.argv[1]
setLogY = int(sys.argv[2])
inputStackDir = sys.argv[3]
nonStack = False
if(".root" in inputStackDir):
    nonStack = True
outputDir = sys.argv[4]
backgroundStacks = sys.argv[5]
backgroundStackList = sys.argv[6]
bgStackDir = sys.argv[7]
flavor = sys.argv[8]
prefix = sys.argv[9]

with open(backgroundStackList) as f:
    lines = f.read().splitlines()

backgroundPlotNames = collections.OrderedDict()
backgrounds = sets.Set()

lineNum=0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    cleanLine = line.strip()
    fullBGname = cleanLine.split()[0]
    legendName = cleanLine.split()[4]
    backgroundName = fullBGname.split("/")[1]
    backgroundPlotNames[legendName] = backgroundName
    backgrounds.add(legendName)

if nonStack:
    saveHistsTDir(inputStackDir,backgroundStacks,outputDir,"",prefix,"backgrounds", 1, flavor, setLogY)
else:
    saveHistsTStack(inputStackDir,backgroundStacks,outputDir,"",prefix,"backgrounds", 1, flavor, setLogY)
#saveHists(ROOT.TFile.Open(sys.argv[1], "read"),sys.argv[2],sys.argv[3],"", sys.argv[4], [wrMass,nuMass], eventsWeight, sys.argv[5])
#def saveHists(folder,directory="",prefix="",bg="simple", eventsWeight=1.0, dataType = "MC", setLog=0):


print "DONE"
sys.stdout.flush()
