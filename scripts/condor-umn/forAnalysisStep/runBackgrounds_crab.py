import sys
import os
import subprocess
import fileinput
from shutil import copyfile

backgroundListDir = "../../../samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
baseCfgDir = "./"
baseCfg = "WR_M-UNDEF_ToLNu_M-UNDEF_Analysis_MuMuJJ_cfg.py"
baseCrabCfgDir = "./"
baseCrabCfg = "crab_cfg_backgrounds.py"
backgroundsDataSetList = "all_datasets_short.txt"
backgroundsDataSetListDir = "../../../samples/backgrounds/"

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
with open(backgroundsDataSetListDir+backgroundsDataSetList) as f:
    lines = f.read().splitlines()
for backgroundDataSet in lines:
    background = backgroundDataSet.split("/")[1]
    print background
    newCfgName = "background"+"_"+background+"_WR_M-"+str(2400)+"_LNu_M-"+str(240)+"_cfg.py"
    newCfg = baseCfgDir+newCfgName

    massCuts = " ; s/MWRUCUT/"+str(2000)+"/g ; s/MWRLCUT/"+str(2800)+"/g\' "
    if("amcatnlo" in background):
        print "IS AN AMC@NLO BG"
        setCfg = "sed \'s/AMCATNLO/True/g"+massCuts+baseCfgDir+baseCfg+" > "+newCfg
    else:
        setCfg = "sed \'s/AMCATNLO/False/g"+massCuts+baseCfgDir+baseCfg+" > "+newCfg

    print "creating configurations"
    subprocess.call(setCfg, shell=True)


    newCrabCfgName = "mass"+"_"+background
    newCrabCfg = baseCrabCfgDir+newCrabCfgName+".py"

    setCrabCfg = "sed 's|DATASET|"+backgroundDataSet+"|g ; s|PYTHONCFG|"+newCfg+"|g ; s|CHANNEL|"+background+"|g' "+baseCrabCfgDir+baseCrabCfg+" > "+newCrabCfg
    #print setCrabCfg
    subprocess.call(setCrabCfg, shell=True)

    crabCommand = "crab submit --config="+newCrabCfg
    print crabCommand
    subprocess.call(crabCommand, shell=True)
