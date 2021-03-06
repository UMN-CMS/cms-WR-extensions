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

def drawMultipleGrid(hists,outname,limits=[],setLogY=False,setLogZ=False, ncols = 3, width=1500,height=1100):
    c = ROOT.TCanvas("c", "c", width,height)
    nhists = len(hists)
    nrows = (nhists-1)/ncols+1
    c.Divide(ncols,nrows)
    
    if len(limits) == 2:
        limits = [limits]*nhists
    if len(limits) == ncols and len(limits[0]) == 2:
        limits = limits*nrows

    for pad in range(len(hists)):
        p = c.cd(pad +1)
        if setLogY: p.SetLogy()
        if setLogZ: p.SetLogz()
        if limits: hists[pad].GetZaxis().SetRangeUser(limits[pad][0], limits[pad][1])
        hists[pad].Draw("colz")
    
    c.SaveAs(outname)

def saveHists(file,directory="",prefix="",filter="",bg="simple",massPoint=[1000,100], eventsWeight=1.0, dataType = "MC", setLog=0):
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
            saveHists(dir,directory=newDir, prefix=prefix,filter=filter,bg=bg,massPoint=massPoint, eventsWeight=eventsWeight, dataType=dataType, setLog=setLog)
        if key.GetClassName() in histObjectNames and filter in prefix:
            hist = file.Get(key.GetName())
            drawoptions = ""
            if key.GetClassName() in hists2d:
                drawoptions = "colz"
	    print "drawHist"
            drawHist(hist,directory+"/"+prefix+"_"+key.GetName()+".png",width=1000,height=800, drawoptions = drawoptions, bg=bg, massPoint=massPoint, eventsWeight=eventsWeight, dataType=dataType, setLogy=setLog)
#            drawHist(hist,directory+"/"+prefix+"_"+key.GetName()+".root",width=1000,height=1000, drawoptions = drawoptions, bg=bg, massPoint=massPoint, eventsWeight=eventsWeight, dataType=dataType, setLogy=setLog)

def getStack(plotName, folder, massPoint):
    #backgroundsDir = "/data/whybee0b/user/aevans/thesis/backgrounds/WR_M-"+str(massPoint[0])+"_LNu_M-"+str(massPoint[1])+"/"
    backgroundsDir = "/uscms_data/d3/mkrohn/WR/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/OutputRoot/PuppiJets_FixMuonWeight/"
    print backgroundsDir+folder+"/"+plotName+".root"
    sys.stdout.flush()
    if not os.path.isfile(backgroundsDir+folder+"/"+plotName+".root"):
        return 0
    file = ROOT.TFile.Open(backgroundsDir+folder+"/"+plotName+".root", "read")
    for key in file.GetListOfKeys():
        if (not key.IsFolder()) and (key.GetName() == plotName):
            print "THSTACK PLOT FOUND"
            sys.stdout.flush()
            return file.Get(key.GetName())
    return 0
def getEventsWeight(file,directory="",prefix="",filter="",inFolder = False):
    hists1d = ["TH1D", "TH1F", "TH1"]
    hists2d = ["TH2D", "TH2F", "TH2"]
    histObjectNames = hists1d + hists2d
    print "Looking for eventsWeight histogram"
    sys.stdout.flush()
    if inFolder:
   #     print "Accessing folder"
        for key in file.GetListOfKeys():
   #         print key.GetName()
            if key.GetClassName() in histObjectNames and filter in prefix and key.GetName() == "eventsWeight":
   #             print "Found events weight" 
                return float(file.Get(key.GetName()).GetBinContent(1))
    else:
  #      print "Not in folder yet"
        for key in file.GetListOfKeys():
   #         print "Looping through keys"
            if key.IsFolder():
                if key.GetName() == "allEvents":
   #                 print "Found Folder"
                    inFolder = True
                dir = file.Get(key.GetName())
                newDir=directory+"/"+key.GetName()
                return getEventsWeight(dir,directory=newDir, prefix=prefix,filter=filter, inFolder = inFolder)
 #   print "UH OH! NO EVENTS WEIGHT PLOT FOUND!"
    return 1.0

def drawHist(hist,name,width=1500,height=1500, drawoptions="",bg="simple",massPoint=[1000,100], eventsWeight=1.0, dataType = "MC", setLogy = 0):
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
#        backgroundStack = getStack(name.split("/")[-1].split("_")[-1][:-5],name.split("/")[-3]+"/"+name.split("/")[-2], massPoint)
        backgroundStack = getStack(name.split("/")[-1].split("_")[-1][:-4],name.split("/")[-3]+"/"+name.split("/")[-2], massPoint)
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
        global colors
        global backgroundPlotNames
	print "colors: ", colors
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

        c.SaveAs(name)
    else:
        print "HISTOGRAM EMPTY!"
        sys.stdout.flush()

def drawMultipleSame(hists,labels,filename,colors=[], width = 500, height = 500, norm = False, xtitle = "", ytitle = "", rebin = 0, leg="top",logy=False):
    hist_max = 0
    if not colors:
        colors = [ROOT.kRed, ROOT.kBlue, ROOT.kBlack]
        colors = colors[:len(hists)]
    for h in hists:
        if rebin:
           h.Rebin(rebin)
        if norm:
           h.Scale(1./h.Integral())
        if h.GetMaximum() > hist_max:
            hist_max = h.GetMaximum()

    canv = ROOT.TCanvas("c","c",width,height)
    if logy:
         canv.SetLogy()
    first = True

    x1 = ROOT.gStyle.GetPadLeftMargin();
    x2 = 1 - ROOT.gStyle.GetPadRightMargin();
    if leg == "top":
       y2 = 1 - ROOT.gStyle.GetPadTopMargin();
       y1 = y2*.9
       y2 = ROOT.gStyle.SetPadTopMargin(y2);
    if leg == "bot":
       y1 = ROOT.gStyle.GetPadBottomMargin();
       y2 = y1 + 0.1
       ROOT.gStyle.SetPadBottomMargin(y1);

    leg = ROOT.TLegend(x1,y1,x2,y2)
    leg.SetFillColor(ROOT.kWhite)
    leg.SetNColumns(len(hists))
    for h,l,c in zip(hists,labels,colors):
        h.SetMaximum(1.2 * hist_max)
        h.SetTitle(l)
        h.SetLineColor(c)
        h.SetLineWidth(3)
        h.GetYaxis().SetTitleOffset(1.5)
        #h.SetOptStat(0)
        if first:
            if xtitle: h.GetXaxis().SetTitle(xtitle)
            if ytitle: h.GetYaxis().SetTitle(ytitle)
            h.Draw()
            first = False
        else:
            h.Draw("same")

        leg.AddEntry(h,l)


    leg.Draw()
    canv.SaveAs(filename)
#############################################################################################
#WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root
#SingleMuon_WR_M-2400_LNu_M-240.root
print "STARTING"
sys.stdout.flush()
backgroundListDir = "/uscms_data/d3/mkrohn/WR/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
#backgroundsList = backgroundListDir+"backgroundStack/backgroundsList_HTbinned_DiBosons.txt"
signalsDir = "/data/whybee0b/user/aevans/thesis/signals/"
with open(backgroundsList) as f:
    lines = f.read().splitlines()

backgrounds = sets.Set()
backgroundPlotNames = collections.OrderedDict()
colors = collections.OrderedDict()
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    backgrounds.add(line.split(':')[2].strip())
    colors[line.split(':')[0].strip()[:-4]] = int(line.split(':')[3].strip())
    backgroundPlotNames[line.split(':')[2].strip()] = line.split(':')[0].strip()[:-4]
    print "test: ", line.split(':')[0].strip()[:-4]

print "Background Plot Names"
print backgroundPlotNames
print "Plot Colors"
print colors
print "Backgrounds"
print backgrounds
sys.stdout.flush()
signalName = sys.argv[1].split("_")
setLogY = int(sys.argv[6])
print "setLogY: ", setLogY
print sys.argv[5]
if (not (sys.argv[5] == "data")):
    wrMass = int(signalName[1][2:])
    nuMass = int(signalName[3][2:])
#    eventsWeight = getEventsWeight(ROOT.TFile.Open(sys.argv[1], "read"),directory=signalsDir)
    print "GOT EVENTSWEIGHT"
    print eventsWeight
sys.stdout.flush()
if (sys.argv[5] == "data"): 
    print "check1: ", signalName[2][2:]
    print "check2: ", signalName[4][2:-5]
#    wrMass = int(signalName[2][2:])
#    nuMass = int(signalName[4][2:-5])
    eventsWeight = 1
saveHists(ROOT.TFile.Open(sys.argv[1], "read"),sys.argv[2],sys.argv[3],"", sys.argv[4], [1000,100], eventsWeight, sys.argv[5], setLogY)
#saveHists(ROOT.TFile.Open(sys.argv[1], "read"),sys.argv[2],sys.argv[3],"", sys.argv[4], [wrMass,nuMass], eventsWeight, sys.argv[5])


print "DONE"
sys.stdout.flush()
