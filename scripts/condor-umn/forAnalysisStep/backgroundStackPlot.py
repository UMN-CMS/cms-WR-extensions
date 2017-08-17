import ROOT
import sys
import os
import subprocess
from shutil import copyfile
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

def saveHists(file,directory="",prefix="",filter=""):
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
            saveHists(dir,directory=newDir, prefix=prefix,filter=filter)
        if key.GetClassName() in histObjectNames and filter in prefix:
            hist = file.Get(key.GetName())
            drawoptions = ""
            if key.GetClassName() in hists2d:
                drawoptions = "colz"
            addHist(hist,directory+"/"+prefix+"_"+key.GetName()+".png", drawoptions = drawoptions)


   

def addHist(hist,name,width=500,height=500, drawoptions=""):
    global stackList
    customROOTstyle()
    if hist.GetName() not in stackList:
        stackList[hist.GetName()] = ROOT.THStack(hist.GetName(),hist.GetName())
        stackList[hist.GetName()].Add(hist)
    else:
        stackList[hist.GetName()].Add(hist)
    #hist.SetLineWidth(2)


stackList = {}
backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cms-WR-extensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
backgroundsROOToutputDir = "/data/whybee0b/user/aevans/"
backgroundsROOToutputSuffix = "background_cfg_"
backgroundROOTdestination = "/home/aevans/public_html/plots/backgrounds/"
#background_cfg_DYJetsToLL_Pt-400To650_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/
subprocess.call("mdkir -p"+backgroundROOTdestination, shell=True)

with open(backgroundsList) as f:
    lines = f.read().splitlines()

backgrounds = []
xsecs = []
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    backgrounds.append(line.split(':')[0].strip())
    xsecs.append(line.split(':')[1].strip())
#print backgrounds
#run over backgrounds
backgroundsRootFiles = {}
for background in backgrounds:
    backgroundsRootFiles[background] = [line for line in os.listdir(backgroundsROOToutputDir+backgroundsROOToutputSuffix+background[:-4]) if ".root" in line]

#print backgroundsRootFiles

for background,files in backgroundsRootFiles.items():
   ahaddOut = background[:-4]+".root"
   ahaddCommand = "ahadd.py "+ahaddOut+" "+backgroundsROOToutputDir+backgroundsROOToutputSuffix+background[:-4]+"/"+"*.root"
   print ahaddCommand
 #  subprocess.call(ahaddCommand, shell=True)   
   saveHists(ROOT.TFile.Open(ahaddOut, "read"),backgroundROOTdestination,background[:-4])

    hist.Draw(drawoptions)
   c.SaveAs(name)








