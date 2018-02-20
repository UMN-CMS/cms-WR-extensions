import sys
import os
import subprocess
from shutil import copyfile

failNumbersFile = "Fatal_elec_skim.txt"
jobsDir = "/data/whybee0b/user/aevans/data_skim_cfg_elec/"

with open(failNumbersFile) as f:
    lines = f.read().splitlines()

for line in lines:
    print "deleting: "+jobsDir+"log/*_"+line+".*"
    deleteLogs = "rm "+jobsDir+"log/*_"+line+".*"
    subprocess.call(deleteLogs,shell=True)
    print "deleting: "+jobsDir+"*_"+line+".root"
    deleteRoots= "rm "+jobsDir+"*_"+line+".root"
    subprocess.call(deleteRoots,shell=True)
    print "deleting: "+jobsDir+"*_"+line+"-pool.root"
    deleteRoots= "rm "+jobsDir+"*_"+line+"-pool.root"
    subprocess.call(deleteRoots,shell=True)
