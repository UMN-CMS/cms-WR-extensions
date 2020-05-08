import sys
import os
import subprocess
from shutil import copyfile

if len(sys.argv) == 2 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    print "=========="
    print "This program takes a path to crab_* submit folders you would like to resubmit."
    print "It resubmits everything in there"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python resubmitAll.py ./runBackgroundMC/Full2018_2020Jan07_145837/"
    print ""
    exit(0)
if len(sys.argv) != 2:
    print "inputs not understood, try --help/-h"
    exit(1)
#proceed with input parsing

pathToCrab = sys.argv[1]

resub = "crab resubmit "


print "RESUBMITTING FOR:"
for line in os.listdir(pathToCrab):
    if "crab_" in line:
        print "FOUND: "+line
        subprocess.call(resub+pathToCrab+line, shell=True)
