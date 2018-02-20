import sys
import os
import subprocess
from shutil import copyfile

dataListDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/data/"
dataList = dataListDir+"SingleElectronC-H.txt"
baseCfgDir = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/scripts/condor-umn/forSkimStep/"
baseCfg = "data_skim_cfg_elec.py"

condorCommand = "condor_filelist.perl --batch 1 --nosubmit "+baseCfg+" "+dataList+" --xrootd --memreq 2000"
print condorCommand
subprocess.call(condorCommand, shell=True)
submitCommand = "condor_submit condor_submit_"+baseCfg[:-3]+".txt"
print submitCommand
#subprocess.call(submitCommand, shell=True)
