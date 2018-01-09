import sys
import os
import subprocess
from shutil import copyfile

dataListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/data/"
dataList = dataListDir+"dataList.txt"
jobsOutDir = "/data/whybee0b/user/aevans/"
outputNameSuffix = "data_skim_cfg_"
hdfsStore = "/hdfs/cms/user/aevans/thesis/data_skim/" 

with open(dataList) as f:
    print "Reading data list"
    sys.stdout.flush()
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

#run over data
dataRootFiles = {}
for data in datas:
    print "copying "+data[:-4]
    sys.stdout.flush()
    #outputFolder = jobsOutDir+outputNameSuffix+data[:-4]+"/"
    outputFolder = jobsOutDir+"data_skim/"
    storeFolder = hdfsStore+data[:-4]
    if not os.path.isdir(outputFolder) :
        print "data: "+data[:-4]+" not found at: "+outputFolder
        sys.stdout.flush()
        continue
    if not os.path.isdir(storeFolder) :
        subprocess.call("mkdir "+storeFolder, shell=True)
    subprocess.call("cp "+outputFolder+"*-pool.root "+storeFolder+"/", shell=True)
    print "finished.  combining plots..."
    sys.stdout.flush()
    rootFiles = ""
    for line in os.listdir(outputFolder):
        if (("-pool" not in line) and (".root" in line)):
            rootFiles+=" "+outputFolder+line
    dataRootFiles[data] = rootFiles
    #print dataRootFiles[data]
#ahadd together eventsWeight histograms
for data,files in dataRootFiles.items():
    print "running ahadd on eventsWeight files"
    sys.stdout.flush()
    ahaddOut = hdfsStore+data[:-4]+"_eventsWeight.root"
    subprocess.call ("rm "+ahaddOut, shell=True)
    ahaddCommand = "ahadd.py "+ahaddOut+" "+files
    subprocess.call(ahaddCommand, shell=True)

