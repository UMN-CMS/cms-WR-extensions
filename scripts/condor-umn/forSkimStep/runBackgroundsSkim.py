import sys
import os
import subprocess
import fileinput
from shutil import copyfile

backgroundListDir = "../../../samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
baseCfgDir = "./"
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
    print background[:-4]
    newCfgName = baseCfg[:-3]+"_"+background[:-4]+".py"
    newCfg = baseCfgDir+newCfgName
    copyfile(baseCfgDir+baseCfg,newCfg)
    if("amcatnlo" in background):
        print "IS AN AMC@NLO BG"
        # redirects STDOUT to the file in question, changes passed in parameter to true for amcatnlo
        for line in fileinput.input(newCfg, inplace = 1): 
              print line.replace("amcatnlo = cms.untracked.bool(False)", "amcatnlo = cms.untracked.bool(True)"),


    condorCommand = "condor_filelist.perl --batch 10 --nosubmit "+newCfg+" "+backgroundListDir+background+" --xrootd --memreq 2500"
    print condorCommand
#    subprocess.call(condorCommand, shell=True)
#    submitCommand = "condor_submit condor_submit_"+newCfgName[:-3]+".txt"
#    subprocess.call(submitCommand, shell=True)
