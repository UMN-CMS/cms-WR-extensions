import sys
import os
import subprocess
from shutil import copyfile

WrMasses=[800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000]
NuMasses=[ 80,  160,  240,  320,  400,  600, 160,  320,  480,  640,  800, 1200, 233,  533,  800, 1067, 1333, 2000]
dataListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/data/"
datasList = dataListDir+"dataList_electron.txt"
baseCfgDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/scripts/condor-umn/forAnalysisStep/"
baseCfg = "data_cfg.py"
dataSkimStore = "/hdfs/cms/user/aevans/thesis/data_skim_cfg_elec/"
nEvents = -1

#opening datas
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
    xsecs.append(line.split(':')[1].strip())

#run over datas
for data in datas:
    for massPoint in range(0, (len(WrMasses)-1)) : 
        print "looping over cut points"
        MWRUCUT = WrMasses[massPoint]*1.3
        MWRLCUT = WrMasses[massPoint]*0.7

        newCfgName = baseCfg[:-3]+"_"+data[:-4]+"_WR_M-"+str(WrMasses[massPoint])+"_LNu_M-"+str(NuMasses[massPoint])+"_cfg.py"
        newCfg = baseCfgDir+newCfgName
        dataFileList = data[:-4]+".txt"
        print "accessing data skim files"
        subprocess.call("ls -d -1 "+dataSkimStore+data[:-4]+"/* > "+dataFileList,shell=True)

        parseFiles = "sed \'s/N_EVENTS/"+str(nEvents)+"/g ; s/MWRUCUT/"+str(MWRUCUT)+"/g ; s/MWRLCUT/"+str(MWRLCUT)+"/g\' "+baseCfgDir+baseCfg+" > "+newCfg
        print "creating configurations"
        subprocess.call(parseFiles, shell=True)

        condorCommand = "condor_filelist.perl --batch 10 --nosubmit "+newCfg+" "+dataFileList+" --xrootd --memreq 2500"
        print "creating jobs"
        subprocess.call(condorCommand, shell=True)
        submitCommand = "condor_submit condor_submit_"+newCfgName[:-7]+".txt"
        print "submitting jobs"
        print submitCommand
        print newCfgName
        subprocess.call(submitCommand, shell=True)
    
