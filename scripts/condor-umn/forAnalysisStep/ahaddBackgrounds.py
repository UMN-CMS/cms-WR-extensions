import sys
import os
import subprocess
from shutil import copyfile



backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cms-WR-extensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
backgroundsROOToutputDir = "/data/whybee0b/user/aevans/"
backgroundsROOToutputSuffix = "background_cfg_"
backgroundROOTdestination = "/data/whybee0b/user/aevans/thesis/backgrounds/"
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
    #xsecs.append(line.split(':')[1].strip())
print backgrounds
#run over backgrounds
backgroundsRootFiles = {}
for background in backgrounds:
    backgroundsRootFiles[background] = [line for line in os.listdir(backgroundsROOToutputDir+backgroundsROOToutputSuffix+background[:-4]) if ".root" in line]

#print backgroundsRootFiles

for background,files in backgroundsRootFiles.items():
   ahaddOut = backgroundROOTdestination+background[:-4]+".root"
   ahaddCommand = "ahadd.py "+ahaddOut+" "+backgroundsROOToutputDir+backgroundsROOToutputSuffix+background[:-4]+"/"+"*.root"
   subprocess.call(ahaddCommand, shell=True)   
