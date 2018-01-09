import sys
import os
import subprocess
from shutil import copyfile

dataListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/data/"
dataList = dataListDir+"SingleMuon.txt"
baseCfgDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/scripts/condor-umn/forSkimStep/"
baseCfg = "data_skim_cfg.py"

condorCommand = "condor_filelist.perl --batch 1 --nosubmit "+baseCfg+" "+dataList+" --xrootd --memreq 2500"
print condorCommand
subprocess.call(condorCommand, shell=True)
submitCommand = "condor_submit condor_submit_"+baseCfg[:-7]+".txt"
subprocess.call(submitCommand, shell=True)
