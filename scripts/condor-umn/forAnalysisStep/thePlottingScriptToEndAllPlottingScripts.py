import ROOT
import sys
import datetime
import subprocess
import os
import copy
import sets
import collections

"""
Style options mostly from CMS's tdrStyle.C
"""
#def customROOTstyle() :
#    ROOT.gROOT.SetBatch(True)
#    ROOT.gStyle.SetOptTitle(True)
#    ROOT.gStyle.SetOptStat(1111)
#    ROOT.gStyle.SetPadTopMargin(0.06);
#    ROOT.gStyle.SetPadBottomMargin(0.13);
#    ROOT.gStyle.SetPadLeftMargin(0.12);
#    ROOT.gStyle.SetPadRightMargin(.15)
#    ROOT.gStyle.SetLabelColor(1, "XYZ");
#    ROOT.gStyle.SetLabelFont(42, "XYZ");
#    ROOT.gStyle.SetLabelOffset(0.007, "XYZ");
#    ROOT.gStyle.SetLabelSize(0.05, "XYZ");
#    ROOT.gStyle.SetTitleSize(0.05, "XYZ");
#    ROOT.gStyle.SetTitleOffset(1.0, "X");
#    ROOT.gStyle.SetTitleOffset(1.1, "Y");
#    ROOT.gStyle.SetTitleOffset(1.0, "Z");
#    ROOT.gStyle.SetAxisColor(1, "XYZ");
#    ROOT.gStyle.SetStripDecimals(True);
#    ROOT.gStyle.SetTickLength(0.03, "XYZ");
#    ROOT.gStyle.SetNdivisions(510, "XYZ");
#    ROOT.gStyle.SetPadTickX(0);
#    ROOT.gStyle.SetPadTickY(0);
#    ROOT.gStyle.SetMarkerStyle(20);
#    ROOT.gStyle.SetHistLineColor(1);
#    ROOT.gStyle.SetHistLineStyle(1);
#    ROOT.gStyle.SetHistLineWidth(3);
#    ROOT.gStyle.SetFrameBorderMode(0);
#    ROOT.gStyle.SetFrameBorderSize(1);
#    ROOT.gStyle.SetFrameFillColor(0);
#    ROOT.gStyle.SetFrameFillStyle(0);
#    ROOT.gStyle.SetFrameLineColor(1);
#    ROOT.gStyle.SetFrameLineStyle(1);
#    ROOT.gStyle.SetFrameLineWidth(1);
#    ROOT.gStyle.SetPalette(55);
#    ROOT.gStyle.SetNumberContours(100);

import numpy as np
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

def saveHists(file,directory="",prefix="",filter="",bg="simple",massPoint=[1000,100], eventsWeight=1.0):
    ROOT.gROOT.SetBatch(True)
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
            saveHists(dir,directory=newDir, prefix=prefix,filter=filter,bg=bg,massPoint=massPoint, eventsWeight=eventsWeight)
        if key.GetClassName() in histObjectNames and filter in prefix:
            hist = file.Get(key.GetName())
            drawoptions = ""
            if key.GetClassName() in hists2d:
                drawoptions = "colz"
            drawHist(hist,directory+"/"+prefix+"_"+key.GetName()+".png",width=1000,height=1000, drawoptions = drawoptions, bg=bg, massPoint=massPoint, eventsWeight=eventsWeight)

def getStack(plotName, folder, massPoint):
    backgroundsDir = "/data/whybee0b/user/aevans/thesis/backgrounds/WR_M-"+str(massPoint[0])+"_LNu_M-"+str(massPoint[1])+"/"
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
    ROOT.gROOT.SetBatch(True)
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

def drawHist(hist,name,width=1500,height=1500, drawoptions="",bg="simple",massPoint=[1000,100], eventsWeight=1.0):
#/home/aevans/public_html/plots/21_Aug_2017_14-49-11-CDT//demo/eventsPassingWR2016RECO/WR_M-4000_ToLNu_M-1333_Analysis_MuMuJJ_selectedJetEta.png
    weight = 1.0
    weight *= 35900.0
    weight *= 4.0 #pb just for plotting purposes
    weight /= eventsWeight
                               
    newHist = copy.deepcopy(hist)
    ROOT.gStyle.SetPalette(61)
    ROOT.gROOT.ForceStyle()
    c = ROOT.TCanvas("c","c",width,height)
    ROOT.gStyle.SetPalette(61)
    ROOT.gROOT.ForceStyle()

    #hist.SetLineWidth(2)
    #print bg
    if (bg == "backgrounds"):
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
        print scaleFactor
        sys.stdout.flush()
        if scaleFactor == 0.0 :
            print "NOT USING SILLY SCALE FACTOR"
            sys.stdout.flush()
            scaleFactor = 1.0
        newHist.Scale(scaleFactor)
        newHist.SetLineColor(794)

        if (newHist.GetMaximum() > backgroundCombined.GetMaximum()) :
            newHist.SetMaximum(newHist.GetMaximum()*1.1)
            backgroundStack.SetMaximum(newHist.GetMaximum()*1.1)
        else :
            newHist.SetMaximum(backgroundCombined.GetMaximum()*1.1)
            backgroundStack.SetMaximum(backgroundCombined.GetMaximum()*1.1)
        backgroundStack.SetTitle("")
        newHist.SetTitle("")
        backgroundStack.Draw("HIST")
        newHist.Draw(drawoptions+"same")

        #c.SetLogy()
        global colors
        global backgroundPlotNames
        legend = c.BuildLegend()
        legend.Clear()  #clear the legend to build it back again
        legend.AddEntry(newHist, "Signal MC M_WR "+str(massPoint[0])+" M_NR "+str(massPoint[1]))
        for bg in backgrounds:
            legend.AddEntry(backgroundPlotNames[bg], bg)
        
        #Now build the legend
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
print "STARTING"
sys.stdout.flush()
backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
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

print "Background Plot Names"
print backgroundPlotNames
print "Plot Colors"
print colors
print "Backgrounds"
print backgrounds
sys.stdout.flush()
signalName = sys.argv[1].split("_")
wrMass = int(signalName[1][2:])
nuMass = int(signalName[3][2:])
eventsWeight = getEventsWeight(ROOT.TFile.Open(sys.argv[1], "read"),directory=signalsDir)
print "GOT EVENTSWEIGHT"
print eventsWeight
sys.stdout.flush()
saveHists(ROOT.TFile.Open(sys.argv[1], "read"),sys.argv[2],sys.argv[3],"", sys.argv[4], [wrMass,nuMass], eventsWeight)


print "DONE"
sys.stdout.flush()
