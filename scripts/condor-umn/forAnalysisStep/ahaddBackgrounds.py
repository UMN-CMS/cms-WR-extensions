import sys
import os
import subprocess
from shutil import copyfile

WrMasses=[800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000]
NuMasses=[ 80,  160,  240,  320,  400,  600, 160,  320,  480,  640,  800, 1200, 233,  533,  800, 1067, 1333, 2000]

backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
backgroundsROOToutputDir = "/data/whybee0b/user/aevans/"
backgroundsROOToutputSuffix = "background_"
backgroundROOTdestination = "/data/whybee0b/user/aevans/thesis/backgrounds/"
#background_cfg_DYJetsToLL_Pt-400To650_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/
#subprocess.call("mdkir -p"+backgroundROOTdestination, shell=True)

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
    #xsecs.append(line.split(':')[1].strip())
print backgrounds
#run over backgrounds

#print backgroundsRootFiles

for background in backgrounds:
    for massPoint in range(0, (len(WrMasses)-1)) :
        massSuffix = "_WR_M-"+str(WrMasses[massPoint])+"_LNu_M-"+str(NuMasses[massPoint])
        ahaddOut = backgroundROOTdestination+background[:-4]+massSuffix+".root"
        removePrevious = "rm "+ahaddOut
        print removePrevious
        subprocess.call(removePrevious, shell=True)
        ahaddCommand = "ahadd.py "+ahaddOut+" "+backgroundsROOToutputDir+backgroundsROOToutputSuffix+background[:-4]+massSuffix+"/"+"*.root"
        print ahaddCommand
        subprocess.call(ahaddCommand, shell=True)   
