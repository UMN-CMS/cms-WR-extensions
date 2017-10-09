import sys
import os
import subprocess
from shutil import copyfile

backgroundListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/backgrounds/"
backgroundsList = backgroundListDir+"backgroundStack/backgroundsList.txt"
jobsOutDir = "/data/whybee0b/user/aevans/"
outputNameSuffix = "background_skim_cfg_"
hdfsStore = "/hdfs/cms/user/aevans/thesis/background_skim/" 

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
backgroundsRootFiles = {}
for background in backgrounds:
    outputFolder = jobsOutDir+outputNameSuffix+background[:-4]+"/"
    storeFolder = hdfsStore+background[:-4]
    if not os.path.isdir(outputFolder) :
        print "background: "+background[:-4]+" not found at: "+folder
        continue
    if not os.path.isdir(storeFolder) :
        subprocess.call("mkdir "+storeFolder, shell=True)
    print "copying "+background[:-4]
    subprocess.call("cp "+outputFolder+outputNameSuffix+background[:-4]+"*-pool.root "+storeFolder+"/", shell=True)
    print "finished.  combining plots..."
    rootFiles = ""
    for line in os.listdir(outputFolder):
        if (("-pool" not in line) and (".root" in line)):
            rootFiles+=" "+outputFolder+line
    backgroundsRootFiles[background] = rootFiles
    #print backgroundsRootFiles[background]
#ahadd together eventsWeight histograms
for background,files in backgroundsRootFiles.items():
    ahaddOut = hdfsStore+background[:-4]+"_eventsWeight.root"
    subprocess.call ("rm "+ahaddOut, shell=True)
    ahaddCommand = "ahadd.py "+ahaddOut+" "+files
    subprocess.call(ahaddCommand, shell=True)

