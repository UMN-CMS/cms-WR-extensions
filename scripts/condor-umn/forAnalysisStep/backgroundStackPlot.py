import ROOT
import sys
import os
import subprocess
from shutil import copyfile
import copy
import collections
"""
Style options mostly from CMS's tdrStyle.C
"""
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
        print file.GetName()
        for key in file.GetListOfKeys():
            print key.GetName()
            if key.GetClassName() in histObjectNames and filter in prefix and key.GetName() == "eventsWeight":
                print "Found events weight" 
                print float(file.Get(key.GetName()).GetBinContent(1))
                print float(file.Get(key.GetName()).GetBinContent(0))
                print float(file.Get(key.GetName()).GetBinContent(2))
                return float(file.Get(key.GetName()).GetBinContent(1))
    else:
        print "Not in folder yet"
        for key in file.GetListOfKeys():
            print "Looping through keys"
            if key.IsFolder():
                print key.GetName()
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
    global colors
    ROOT.gStyle.SetPalette(55)
    hist.SetTitle(backgroundName)
    hist.Scale(weight)
    hist.SetFillColor(colors[backgroundName])
    hist.SetLineColor(colors[backgroundName])
    print "ADDING HIST"
    print backgroundName
    print hist.GetName()
    hist.SetName(backgroundName)
    if name not in stackList:
   #     print "New Plot!"
        stackList[name] = []
        ROOT.gStyle.SetPalette(55)
        stackList[name].append(copy.deepcopy(hist))
    else:
  #      print "Stacking!"
        ROOT.gStyle.SetPalette(55)
        stackList[name].append(copy.deepcopy(hist))
    #hist.SetLineWidth(2)

#####################################################################################################################################
#WrMasses=[800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000]
#NuMasses=[ 80,  160,  240,  320,  400,  600, 160,  320,  480,  640,  800, 1200, 233,  533,  800, 1067, 1333, 2000]
WrMasses=[2400]
NuMasses=[240]
integratedLuminosity = 35900.0
lumiAdjust = 1.0 #in case you want to compare with only a fraction of the 2016 data *= 0.6641282341721065  #FUDGE FACTOR CAUSE I'M MISSING EVENTS FOR THE MUON DATA 5.339658e8 / 804010088
lumiAdjust *= 0.6641282341721065  #FUDGE FACTOR CAUSE I'M MISSING EVENTS FOR THE MUON DATA 5.339658e8 / 804010088
integratedLuminosity *= lumiAdjust
stackList = collections.OrderedDict()
backgroundListDir = "../../../samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
backgroundsROOToutputDir = "/afs/cern.ch/work/a/aevans/public/thesis/backgrounds/"
#backgroundsROOToutputSuffix = "background_"
backgroundsROOToutputSuffix = ""
backgroundROOTdestination = "/afs/cern.ch/work/a/aevans/public/thesis/backgrounds/"
eventsWeightsDir = "/afs/cern.ch/work/a/aevans/public/thesis/backgrounds/"
#background_cfg_DYJetsToLL_Pt-400To650_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/
#subprocess.call("mkdir -p"+backgroundROOTdestination, shell=True)

with open(backgroundsList) as f:
    lines = f.read().splitlines()

xsecs = collections.OrderedDict()
colors = collections.OrderedDict()
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    xsecs[line.split(':')[0].strip()] = float(line.split(':')[1].strip().split("+")[0])
    colors[line.split(':')[0].strip()[:-4]] = int(line.split(':')[3].strip())
    #print line.split(':')[2].strip()

print "NEW COLOR"
print colors
#print backgrounds
#run over backgrounds
nmasses = 0
if (len(WrMasses) - 1 == 0 ):
    nmasses = 1
elif (len(WrMasses) - 1 > 0 ):
    nmasses = len(WrMasses) - 1
else:
    nmasses = 0  #this shouldn't end up happening

#print backgroundsRootFiles
for massPoint in range(0, nmasses):
    stackList.clear()
    #massSuffix = "_WR_M-"+str(WrMasses[massPoint])+"_LNu_M-"+str(NuMasses[massPoint])
    massSuffix = ""
    massName = ""
    massDir = backgroundROOTdestination+massName+"/"
    pos = 1
    end = len(xsecs)
    if(not (os.path.isdir(massDir))):
        subprocess.call(["mkdir", massDir])
    for background,xsec in xsecs.items():
        ahaddOut = backgroundROOTdestination+background[:-4]+massSuffix+".root"
        backgroundEventsWeight = eventsWeightsDir+background[:-4]+".root"
        #backgroundEventsWeight = eventsWeightsDir+background[:-4]+"_eventsWeight.root"
        print backgroundEventsWeight
     #  subprocess.call(ahaddCommand, shell=True)   
     #   if pos == end:
     #       print "LAST ROUND!"
      #  print pos
       # print background[:-4]+massSuffix
        weight = 1.0
        weight *= integratedLuminosity
        weight *= xsecs[background]
        print "Lumi*xsec="+str(weight)
        print "LOOKING FOR EVENTS WEIGHT IN FILE"
        eventsWeight = 0
        eventsWeight = getEventsWeight(ROOT.TFile.Open(backgroundEventsWeight, "read"),directory=eventsWeightsDir)
        if (eventsWeight == 0):
            print "BACKGROUND HAS ZERO EVENTS WEIGHT"
            continue
        print "Found # events:"+str(eventsWeight)
        weight /= eventsWeight
        print "DONE CALCULATING"
        print "Scale: "+str(weight)
        saveHists(weight,background[:-4],ROOT.TFile.Open(ahaddOut, "read"),directory=massDir)
        pos+=1
    
    #Loop over stacks and make save stackhists
    
    #c = ROOT.TCanvas("c","c",1000,1000)
    for plot,stack in stackList.items():
        stackHist = ROOT.THStack(plot.split("/")[-1][:-5],plot.split("/")[-1][:-5])
        pos = 1
#        print stack.GetName()
        for hist in stack:
 #           print hist.__class__.__name__
            #myHist = copy.deepcopy(hist)
        #    print "Adding"
            print "STACK TIME"
            print hist.GetName()
            #hist.SetFillColor(pos)
            hist.Draw("HIST")
            stackHist.Add(hist)
            if pos == 9:
                pos+=2
            else:
                pos+=1
        ROOT.gStyle.SetPalette(55)
        #stackHist.Draw()
 #       print "THIS MANY HISTS"
 #       print stackHist.GetNhists()
        #c.BuildLegend()
        #c.SaveAs(plot)
        stackHist.SaveAs(plot)






