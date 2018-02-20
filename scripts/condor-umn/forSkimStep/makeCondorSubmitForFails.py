import sys
import os
import subprocess
from shutil import copyfile

files = "condor_submit_data_skim_cfg_elec.txt"
failNumbersFile = "Fatal_elec_skim.txt"

with open(failNumbersFile) as f:
    failNumbers = f.read().splitlines()
#print failNumbers
with open(files) as f:
    lines = f.read().splitlines()

iJustPrintedQueue = False
for lineNum, line in enumerate(lines):
    if "Argument" not in line:
        if "Queue" in line:
            if not iJustPrintedQueue:
                print line
                iJustPrintedQueue = True
        else:
            print line
            iJustPrintedQueue = False
    else:
        if line.split(".err")[0].split("_")[-1] in failNumbers:
            #print line.split(".err")[0].split("_")[-1]
            print line
            iJustPrintedQueue = False
