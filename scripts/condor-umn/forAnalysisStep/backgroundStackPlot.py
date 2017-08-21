import ROOT
import sys
import os
import subprocess
from shutil import copyfile
import copy
"""
Style options mostly from CMS's tdrStyle.C
"""
def customROOTstyle() :
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptTitle(True)
    ROOT.gStyle.SetOptStat(1111)
    ROOT.gStyle.SetPadTopMargin(0.06);
    ROOT.gStyle.SetPadBottomMargin(0.13);
    ROOT.gStyle.SetPadLeftMargin(0.12);
    ROOT.gStyle.SetPadRightMargin(.15)
    ROOT.gStyle.SetLabelColor(1, "XYZ");
    ROOT.gStyle.SetLabelFont(42, "XYZ");
    ROOT.gStyle.SetLabelOffset(0.007, "XYZ");
    ROOT.gStyle.SetLabelSize(0.05, "XYZ");
    ROOT.gStyle.SetTitleSize(0.05, "XYZ");
    ROOT.gStyle.SetTitleOffset(1.0, "X");
    ROOT.gStyle.SetTitleOffset(1.1, "Y");
    ROOT.gStyle.SetTitleOffset(1.0, "Z");
    ROOT.gStyle.SetAxisColor(1, "XYZ");
    ROOT.gStyle.SetStripDecimals(True);
    ROOT.gStyle.SetTickLength(0.03, "XYZ");
    ROOT.gStyle.SetNdivisions(510, "XYZ");
    ROOT.gStyle.SetPadTickX(0);
    ROOT.gStyle.SetPadTickY(0);
    ROOT.gStyle.SetMarkerStyle(20);
    ROOT.gStyle.SetHistLineColor(1);
    ROOT.gStyle.SetHistLineStyle(1);
    ROOT.gStyle.SetHistLineWidth(3);
    ROOT.gStyle.SetFrameBorderMode(0);
    ROOT.gStyle.SetFrameBorderSize(1);
    ROOT.gStyle.SetFrameFillColor(0);
    ROOT.gStyle.SetFrameFillStyle(0);
    ROOT.gStyle.SetFrameLineColor(1);
    ROOT.gStyle.SetFrameLineStyle(1);
    ROOT.gStyle.SetFrameLineWidth(1);
    ROOT.gStyle.SetPalette(55);
    ROOT.gStyle.SetNumberContours(100);
import numpy as np
def customPalette(zeropoint = 0.5):
    Number = 3;
    Red    = np.array([0  ,  100,  110],dtype=float)/255.
    Green  = np.array([0  ,  255,  0], dtype=float)/255.
    Blue   = np.array([99 ,  100,  2], dtype=float)/255.
    Length = np.array([0.0,  zeropoint, 1.0], dtype=float)
    nb=100;
    ROOT.TColor.CreateGradientColorTable(Number,Length,Red,Green,Blue,nb)

def saveHists(weight,backgroundName,file,directory="",prefix="",filter=""):
    customROOTstyle()
    ROOT.gROOT.SetBatch(True)
    hists1d = ["TH1D", "TH1F", "TH1"]
    hists2d = ["TH2D", "TH2F", "TH2"]
    histObjectNames = hists1d + hists2d
    for key in file.GetListOfKeys():
        if key.IsFolder():
            dir = file.Get(key.GetName())
            newDir=directory+"/"+key.GetName()
            if(not (os.path.isdir(newDir))):
                subprocess.call(["mkdir", newDir])
            saveHists(weight,backgroundName,dir,directory=newDir, prefix=prefix,filter=filter)
        if key.GetClassName() in histObjectNames and filter in prefix:
            hist = file.Get(key.GetName())
            drawoptions = ""
            if key.GetClassName() in hists2d:
                drawoptions = "colz"
       #     hist.SetFillColor(color)
            addHist(weight,backgroundName,hist,directory+"/"+key.GetName()+".root", width=1000, height=1000, drawoptions = drawoptions)

def getEventsWeight(file,directory="",prefix="",filter="",inFolder = False):
    ROOT.gROOT.SetBatch(True)
    hists1d = ["TH1D", "TH1F", "TH1"]
    hists2d = ["TH2D", "TH2F", "TH2"]
    histObjectNames = hists1d + hists2d
    print "Looking for eventsWeight histogram"
    if inFolder:
        print "Accessing folder"
        for key in file.GetListOfKeys():
            print key.GetName()
            if key.GetClassName() in histObjectNames and filter in prefix and key.GetName() == "eventsWeight":
                print "Found events weight" 
                return float(file.Get(key.GetName()).GetBinContent(1))
    else:
        print "Not in folder yet"
        for key in file.GetListOfKeys():
            print "Looping through keys"
            if key.IsFolder():
                if key.GetName() == "allEvents":
                    print "Found Folder"
                    inFolder = True
                dir = file.Get(key.GetName())
                newDir=directory+"/"+key.GetName()
                return getEventsWeight(dir,directory=newDir, prefix=prefix,filter=filter, inFolder = inFolder)
    print "UH OH! NO EVENTS WEIGHT PLOT FOUND!"
    return 1.0

   

def addHist(weight,backgroundName,hist,name,width=500,height=500, drawoptions=""):
    global stackList
    customROOTstyle()
    ROOT.gStyle.SetPalette(55)
    hist.SetTitle(backgroundName)
    hist.Scale(weight)
    print hist.GetName()
    if name not in stackList:
        print "New Plot!"
        stackList[name] = []
        ROOT.gStyle.SetPalette(55)
        stackList[name].append(copy.deepcopy(hist))
    else:
        print "Stacking!"
        ROOT.gStyle.SetPalette(55)
        stackList[name].append(copy.deepcopy(hist))
    #hist.SetLineWidth(2)

integratedLuminosity = 35900.0
stackList = {}
backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cms-WR-extensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
backgroundsROOToutputDir = "/data/whybee0b/user/aevans/"
backgroundsROOToutputSuffix = "background_cfg_"
backgroundROOTdestination = "/home/aevans/public_html/plots/backgrounds/"
#background_cfg_DYJetsToLL_Pt-400To650_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/
#subprocess.call("mkdir -p"+backgroundROOTdestination, shell=True)

with open(backgroundsList) as f:
    lines = f.read().splitlines()

xsecs = {}
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    xsecs[line.split(':')[0].strip()] = float(line.split(':')[1].strip().split("+")[0])
#print backgrounds
#run over backgrounds
backgroundsRootFiles = {}
for background,xsec in xsecs.items():
    #if 
    backgroundsRootFiles[background] = backgroundROOTdestination+background[:-4]+".root"

#print backgroundsRootFiles
pos = 1
end = len(backgroundsRootFiles)
for background,files in backgroundsRootFiles.items():
    ahaddOut = backgroundROOTdestination+background[:-4]+".root"
 #  subprocess.call(ahaddCommand, shell=True)   
    if pos == end:
        print "LAST ROUND!"
    print pos
    print background[:-4]
    weight = 1.0
    weight *= integratedLuminosity
    weight *= xsecs[background]
    print "LOOKING FOR EVENTS WEIGHT IN FILE"
    weight /= getEventsWeight(ROOT.TFile.Open(ahaddOut, "read"),directory=backgroundROOTdestination)
    print "DONE CALCULATING"
    saveHists(weight,background[:-4],ROOT.TFile.Open(ahaddOut, "read"),directory=backgroundROOTdestination)
    pos+=1

#Loop over stacks and make save stackhists

#c = ROOT.TCanvas("c","c",1000,1000)
for plot,stack in stackList.items():
    customROOTstyle()
    stackHist = ROOT.THStack(plot.split("/")[-1][:-5],plot.split("/")[-1][:-5])
    customROOTstyle()
    pos = 1
    for hist in stack:
        print hist.__class__.__name__
        #myHist = copy.deepcopy(hist)
        print "Adding"
        hist.SetFillColor(pos)
        stackHist.Add(hist)
        if pos == 9:
            pos+=2
        else:
            pos+=1
    ROOT.gStyle.SetPalette(55)
    customROOTstyle()
    #stackHist.Draw()
    customROOTstyle()
    print "THIS MANY HISTS"
    print stackHist.GetNhists()
    #c.BuildLegend()
    #c.SaveAs(plot)
    stackHist.SaveAs(plot)






