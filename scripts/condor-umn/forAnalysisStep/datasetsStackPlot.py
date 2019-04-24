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
	    if hist.Integral() == 0: continue
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
    hist.SetName(backgroundName)
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


#####################################################################################################################################################################
#INPUT FILTERING
if len(sys.argv) == 2 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    print "=========="
    print "This program takes a directory containing MC or data and builds stack plots of them together."
    print "It is driven by a text file specifying which datasets to look at as well as other details"
    print "In order, please specify:"
    print "Text file contain list of datasets:"
    print "Directory where the dataset ROOTs are stored:"
    print "Destination directory to put combined files in:"
    print "Samples year"
    print "OPTIONAL: Lumi adjustment factor (35.9 fb-1 scaling default)"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python datasetsStackPlot.py ../../../samples/backgrounds/fullBackgroundDatasetList_no_ext.txt /afs/cern.ch/work/a/aevans/public/thesis/backgroundStacks/ /afs/cern.ch/work/a/aevans/public/thesis/backgroundPlots/ 2016 1.0"
    print ""
    exit(0)
if len(sys.argv) != 6:
    print "inputs not understood, try --help/-h"
    exit(1)
#proceed with input parsing

backgroundsList = sys.argv[1]
backgroundsROOToutputDir = sys.argv[2]
backgroundsROOTdestination = sys.argv[3]
eventsWeightsDir = backgroundsROOToutputDir
year = sys.argv[4]
lumiAdjust = sys.argv[5]
#in case you want to compare with only a fraction of the 2016 data *= 0.6641282341721065  #FUDGE FACTOR CAUSE I'M MISSING EVENTS FOR THE MUON DATA 5.339658e8 / 804010088
#lumiAdjust *= 0.6641282341721065  #FUDGE FACTOR CAUSE I'M MISSING EVENTS FOR THE MUON DATA 5.339658e8 / 804010088
if(year=="2016"):
    integratedLuminosity = 35900.0
elif(year=="2017"):
#    integratedLuminosity = 13000.0
    integratedLuminosity = 41500.0
elif(year=="2018"):
#    integratedLuminosity = 25000.0
#    integratedLuminosity = 31930.0
    integratedLuminosity = 59970.0
integratedLuminosity *= float(lumiAdjust)

with open(backgroundsList) as f:
    lines = f.read().splitlines()

xsecs = collections.OrderedDict()
colors = collections.OrderedDict()
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    xsec = 0.0
    if ("SingleMuon" in line.split()[0] or "SingleElectron" in line.split()[0] or "EGamma" in line.split()[0]):
        name = line.split()[0].strip().split("/")[1]+"--"+line.split()[0].strip().split("/")[2]
        colors[name] = int(line.split()[5].strip())
        print "Found: "+name
	xsecs[name] = 1.0
    else:
        xsecLine = line.split()[3].strip().split("+")[0]
        if "*" in xsecLine:
            xsec = float(xsecLine.split("*")[0])*float(xsecLine.split("*")[1])
        else:
            xsec = float(xsecLine)
        if xsec == 0.0:
            print "ERROR MALFORMED XSEC: "
            print xsecLine
            exit(1)
        name = line.split()[0].strip().split("/")[1]
        xsecs[name] = xsec
#        colors[name] = int(line.split()[5].strip())
        print "Found: "+name
    #print line.split(':')[2].strip()
    
colors = {'QCD_HT100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8': ROOT.kOrange,
          'QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8': ROOT.kOrange,
          'QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8': ROOT.kOrange,
          'QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8': ROOT.kOrange,
          'QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8': ROOT.kOrange,
          'QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8': ROOT.kOrange,
          'QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kOrange,
          'WW_TuneCUETP8M1_13TeV-pythia8': ROOT.kRed-2,
          'WZ_TuneCUETP8M1_13TeV-pythia8': ROOT.kRed-2,
          'ZZ_TuneCUETP8M1_13TeV-pythia8': ROOT.kRed-2,
          'WW_TuneCP5_13TeV-pythia8': ROOT.kRed-2,
          'WZ_TuneCP5_13TeV-pythia8': ROOT.kRed-2,
          'ZZ_TuneCP5_13TeV-pythia8': ROOT.kRed-2,
          'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8': ROOT.kRed,
          'ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin': ROOT.kRed,
          'ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin': ROOT.kRed,
          'ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4': ROOT.kRed,
          'ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4': ROOT.kRed,
          'ST_t-channel_antitop_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8': ROOT.kRed,
          'ST_t-channel_top_4f_inclusiveDecays_TuneCP5_13TeV-powhegV2-madspin-pythia8': ROOT.kRed,
          'ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8': ROOT.kRed,
          'ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8': ROOT.kRed,
          'ST_s-channel_4f_leptonDecays_TuneCP5_PSweights_13TeV-amcatnlo-pythia8': ROOT.kRed,
          'ST_t-channel_antitop_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8': ROOT.kRed,
          'ST_t-channel_top_4f_InclusiveDecays_TuneCP5_13TeV-powheg-madspin-pythia8': ROOT.kRed,
	  'ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-madgraph-pythia8': ROOT.kRed,
          'ST_s-channel_top_leptonDecays_13TeV-PSweights_powheg-pythia': ROOT.kRed,
          'ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1': ROOT.kRed,
          'ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1': ROOT.kRed,
          'DYJetsToLL_Zpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_Pt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_Pt-400To650_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_Pt-650ToInf_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kGreen+3,
	  'DYJetsToLL_M-50_HT-100to200_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-1200to2500_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-200to400_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-2500toInf_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-400to600_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-600to800_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-800to1200_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-100to200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-1200to2500_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-200to400_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-2500toInf_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-600to800_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-800to1200_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-200to400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-400to600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-600to800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-800to1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-1200to2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'DYJetsToLL_M-50_HT-2500toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kGreen+3,
          'WJetsToLNu_Wpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_Wpt-50To100_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_Pt-400To600_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_Pt-600ToInf_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-100To200_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-1200To2500_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-2500ToInf_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-200To400_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-600To800_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-800To1200_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8': ROOT.kBlue+2,
          'TT_TuneCUETP8M2T4_13TeV-powheg-pythia8': ROOT.kGray,
          'TTJets_TuneCP5_13TeV-madgraphMLM-pythia8': ROOT.kGray,
          'TTJets_TuneCP5_13TeV-amcatnloFXFX-pythia8': ROOT.kGray,
          'SingleElectron--Run2016B-17Jul2018_ver2-v1'          : 794,
          'SingleElectron--Run2016C-17Jul2018-v1'               : 794,
          'SingleElectron--Run2016D-17Jul2018-v1'               : 794,
          'SingleElectron--Run2016E-17Jul2018-v1'               : 794,
          'SingleElectron--Run2016F-17Jul2018-v1'               : 794,
          'SingleElectron--Run2016G-17Jul2018-v1'               : 794,
          'SingleElectron--Run2016H-17Jul2018-v1'          : 794,
          'SingleElectron--Run2017B-31Mar2018-v1'          : 794,
          'SingleElectron--Run2017C-31Mar2018-v1'          : 794,
          'SingleElectron--Run2017D-31Mar2018-v1'          : 794,
          'SingleElectron--Run2017E-31Mar2018-v1'          : 794,
          'SingleElectron--Run2017F-31Mar2018-v1'          : 794,
          'EGamma--Run2018A-17Sep2018-v2'          : 794,
          'EGamma--Run2018B-17Sep2018-v1'          : 794,
          'EGamma--Run2018C-17Sep2018-v1'          : 794,
          'EGamma--Run2018D-PromptReco-v2'          : 794,
          'SingleMuon--Run2016B-17Jul2018_ver2-v1'          : 794,
          'SingleMuon--Run2016C-17Jul2018-v1'          : 794,
          'SingleMuon--Run2016D-17Jul2018-v1'          : 794,
          'SingleMuon--Run2016E-17Jul2018-v1'          : 794,
          'SingleMuon--Run2016F-17Jul2018-v1'          : 794,
          'SingleMuon--Run2016G-17Jul2018-v1'          : 794,
          'SingleMuon--Run2016H-17Jul2018-v1'          : 794,
          'SingleMuon--Run2017B-31Mar2018-v1'          : 794,
          'SingleMuon--Run2017C-31Mar2018-v1'          : 794,
          'SingleMuon--Run2017D-31Mar2018-v1'          : 794,
          'SingleMuon--Run2017E-31Mar2018-v1'          : 794,
          'SingleMuon--Run2017F-31Mar2018-v1'          : 794,
          'SingleMuon--Run2018A-17Sep2018-v2'          : 794,
          'SingleMuon--Run2018B-17Sep2018-v1'          : 794,
          'SingleMuon--Run2018C-17Sep2018-v1'          : 794,
          'SingleMuon--Run2018D-PromptReco-v2'          : 794,
          'WR_M-5000_ToLNu_M-1650'                              : ROOT.kBlack,
          'WR_M-5000_ToLNu_M-1000'                              : ROOT.kBlack,
          'WR_M-5000_ToLNu_M-500'                              : ROOT.kBlack,
          'WR_M-4500_ToLNu_M-1500'                              : ROOT.kBlack,
          'WR_M-4500_ToLNu_M-900'                              : ROOT.kBlack,
          'WR_M-4500_ToLNu_M-450'                              : ROOT.kBlack,
          'WR_M-4000_ToLNu_M-1300'                              : ROOT.kBlack,
          'WR_M-4000_ToLNu_M-800'                              : ROOT.kBlack,
          'WR_M-4000_ToLNu_M-400'                              : ROOT.kBlack,
          'WR_M-3500_ToLNu_M-1150'                              : ROOT.kBlack,
          'WR_M-3500_ToLNu_M-700'                              : ROOT.kBlack,
          'WR_M-3500_ToLNu_M-350'                              : ROOT.kBlack,
          'WR_M-3000_ToLNu_M-1000'                              : ROOT.kBlack,
          'WR_M-3000_ToLNu_M-600'                              : ROOT.kBlack,
          'WR_M-3000_ToLNu_M-300'                              : ROOT.kBlack,
          'WR_M-2500_ToLNu_M-800'                              : ROOT.kBlack,
          'WR_M-2500_ToLNu_M-500'                              : ROOT.kBlack,
          'WR_M-2500_ToLNu_M-250'                              : ROOT.kBlack,
          'WR_M-2000_ToLNu_M-650'                              : ROOT.kBlack,
          'WR_M-2000_ToLNu_M-400'                              : ROOT.kBlack,
          'WR_M-2000_ToLNu_M-200'                              : ROOT.kBlack,
          'WR_M-1500_ToLNu_M-500'                              : ROOT.kBlack,
          'WR_M-1500_ToLNu_M-300'                              : ROOT.kBlack,
          'WR_M-1500_ToLNu_M-150'                              : ROOT.kBlack,
          'WR_M-1000_ToLNu_M-300'                              : ROOT.kBlack,
          'WR_M-1000_ToLNu_M-200'                              : ROOT.kBlack,
          'WR_M-1000_ToLNu_M-100'                              : ROOT.kBlack
         }
    

#print "NEW COLOR"
#print colors
#print backgrounds
#run over backgrounds
stackList = collections.OrderedDict()
stackList.clear()



for background,xsec in xsecs.items():
#for background in colors: #HERE WE LOOP OVER THE KEYS IN COLORS, WHICH IS ALWAYS FILLED AND FOR BACKGROUNDS WILL HAVE A VALUE PAIRING IN XSECS
    ahaddOut = backgroundsROOToutputDir+background+".root"
    backgroundEventsWeight = eventsWeightsDir+background+".root"
    print backgroundEventsWeight
    weight = 1.0
    if ("SingleElectron" not in background and "SingleMuon" not in background and "EGamma" not in background):  #MC ONLY
        weight *= integratedLuminosity
        weight *= xsecs[background]
        print "Lumi*xsec="+str(weight)
        print "LOOKING FOR EVENTS WEIGHT IN FILE"
        eventsWeight = 0
        eventsWeight = getEventsWeight(ROOT.TFile.Open(backgroundEventsWeight, "read"),directory=backgroundsROOToutputDir)
        if (eventsWeight == 0):
            print "BACKGROUND HAS ZERO EVENTS WEIGHT"
            continue
        print "Found # events:"+str(eventsWeight)
        weight /= eventsWeight
	if "tW" in background and (year == "2017" or year == "2018"):
	    print "Reducing ST weight"
	    weight /= 35.1316
        print "DONE CALCULATING"
    print "Scale: "+str(weight)
 #   saveHists(weight,background,ROOT.TFile.Open(ahaddOut, "read"),directory=backgroundsROOToutputDir)
    saveHists(weight,background,ROOT.TFile.Open(ahaddOut, "read"),directory=backgroundsROOTdestination)

#Loop over stacks and make save stackhists

#c = ROOT.TCanvas("c","c",1000,1000)
for plot,stack in stackList.items():
    pos = 1
    dummy = 0
    histType = stack[0].__class__.__name__
    # For 2D just add the histos together
    # So make a clone of 0th one to add rest to
    if "TH2" in histType:
        dummy = stack[0].Clone()
    # For 1D add the histos to a stack
    # So make a dummy stack
    elif "TH1" in histType:
        dummy = ROOT.THStack(plot.split("/")[-1][:-5],plot.split("/")[-1][:-5])
    #stackHist = ROOT.THStack(plot.split("/")[-1][:-5],plot.split("/")[-1][:-5])
    print stack
    print "plot: ", plot
    print "Hist name: ", plot.split("/")[-1][:-5]
    for hist in stack:
        # For 2D, skip the 0th hist since dummy is a clone
        if "TH2" in histType and pos == 1:
            pos+=1
            continue
        print histType 
        #myHist = copy.deepcopy(hist)
    #    print "Adding"
        print "ADDING..."
        print hist.GetName()
	print "Integral: ", hist.Integral()
        #hist.SetFillColor(pos)
        hist.Draw("HIST")
        dummy.Add(hist)
        if pos == 9:
            pos+=2
        else:
            pos+=1
    ROOT.gStyle.SetPalette(55)
#stackHist.Draw()
    if "TH1" in histType:
        print "THIS MANY HISTS IN STACK"
        print dummy.GetNhists()
#c.BuildLegend()
#c.SaveAs(plot)
    dummy.SaveAs(plot)
