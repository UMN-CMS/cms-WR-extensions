import sys
import os
import subprocess
from shutil import copyfile

WrMasses=[800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000]
NuMasses=[ 80,  160,  240,  320,  400,  600, 160,  320,  480,  640,  800, 1200, 233,  533,  800, 1067, 1333, 2000]
backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cms-WR-extensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
baseCfgDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cms-WR-extensions/scripts/condor-umn/forAnalysisStep/"
baseCfg = "background_cfg.py"
backgroundSkimStore = "/hdfs/cms/user/aevans/thesis/background_skim/"
nEvents = -1

#opening backgrounds
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
    for massPoint in range(0, (len(WrMasses)-1)) : 
        print "looping over cut points"
        MWRUCUT = WrMasses[massPoint]*1.3
        MWRLCUT = WrMasses[massPoint]*0.7

        newCfgName = baseCfg[:-3]+"_"+background[:-4]+"_WR_M-"+str(WrMasses[massPoint])+"_LNu_M-"+str(NuMasses[massPoint])+"_cfg.py"
        newCfg = baseCfgDir+newCfgName
        backgroundFileList = background[:-4]+"_files.txt"
        print "accessing background skim files"
        subprocess.call("ls -d -1 "+backgroundSkimStore+background[:-4]+"/* > "+backgroundFileList,shell=True)

        parseFiles = "sed \'s/N_EVENTS/"+str(nEvents)+"/g ; s/MWRUCUT/"+str(MWRUCUT)+"/g ; s/MWRLCUT/"+str(MWRLCUT)+"/g\' "+baseCfgDir+baseCfg+" > "+newCfg
        print "creating configurations"
        subprocess.call(parseFiles, shell=True)

        condorCommand = "condor_filelist.perl --batch 10 --nosubmit "+newCfg+" "+backgroundFileList+" --xrootd --memreq 2500"
        print "creating jobs"
        subprocess.call(condorCommand, shell=True)
        submitCommand = "condor_submit condor_submit_"+newCfgName[:-7]+".txt"
        print "submitting jobs"
        print submitCommand
        print newCfgName
        subprocess.call(submitCommand, shell=True)
    
