import sys
import os
import subprocess
from shutil import copyfile

WrMasses=[800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000]
NuMasses=[ 80,  160,  240,  320,  400,  600, 160,  320,  480,  640,  800, 1200, 233,  533,  800, 1067, 1333, 2000]

dataListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/data/"
datasList = dataListDir+"dataList.txt"
datasROOToutputDir = "/data/whybee0b/user/aevans/"
datasROOToutputSuffix = "data_cfg_"
dataROOTdestination = "/data/whybee0b/user/aevans/thesis/data/"
#background_cfg_DYJetsToLL_Pt-400To650_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/
#subprocess.call("mdkir -p"+backgroundROOTdestination, shell=True)

with open(datasList) as f:
    lines = f.read().splitlines()

datas = []
xsecs = []
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    datas.append(line.split(':')[0].strip())
    #xsecs.append(line.split(':')[1].strip())
print datas
#run over backgrounds

#print backgroundsRootFiles

for data in datas:
    for massPoint in range(0, (len(WrMasses)-1)) :
        massSuffix = "_WR_M-"+str(WrMasses[massPoint])+"_LNu_M-"+str(NuMasses[massPoint])
        ahaddOut = dataROOTdestination+data[:-4]+massSuffix+".root"
        removePrevious = "rm "+ahaddOut
        print removePrevious
        subprocess.call(removePrevious, shell=True)
        ahaddCommand = "ahadd.py "+ahaddOut+" "+datasROOToutputDir+datasROOToutputSuffix+data[:-4]+massSuffix+"/"+"*.root"
        print ahaddCommand
        subprocess.call(ahaddCommand, shell=True)   
