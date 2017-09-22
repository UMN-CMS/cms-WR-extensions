import sys
import os
import subprocess
from shutil import copyfile

backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
baseCfgDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/scripts/condor-umn/forSkimStep/"
baseCfg = "background_skim_cfg.py"

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

#run over backgrounds
for background in backgrounds:
    newCfgName = baseCfg[:-3]+"_"+background[:-4]+".py"
    newCfg = baseCfgDir+newCfgName
    copyfile(baseCfgDir+baseCfg,newCfg)
    condorCommand = "condor_filelist.perl --batch 10 --nosubmit "+newCfg+" "+backgroundListDir+background+" --xrootd --memreq 2500"
    print condorCommand
    subprocess.call(condorCommand, shell=True)
    submitCommand = "condor_submit condor_submit_"+newCfgName[:-3]+".txt"
    subprocess.call(submitCommand, shell=True)
