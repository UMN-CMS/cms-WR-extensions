import sys
import os
import subprocess
from shutil import copyfile

if len(sys.argv) == 2 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    print "=========="
    print "This program takes collections of histograms, ROOT file by ROOT file and \ncombines each set together into one ROOT file"
    print "In order, please specify:"
    print "Text file contain list of datasets:"
    print "Directory where the datasets are stored:"
    print "Destination directory to put combined files in:"
    print "=========="
    print "EXAMPLE:"
    print ""
    print "python ahaddBackgrounds.py ../../../samples/backgrounds/multiCrabTest.txt /eos/cms/store/user/aevans/undefined_2018Jun15_111107 /afs/cern.ch/work/a/aevans/public/thesis/backgrounds/"
    print ""
    exit(0)
if len(sys.argv) != 4:
    print "inputs not understood, try --help/-h"
    exit(1)
#proceed with input parsing

backgroundsList = sys.argv[1]
backgroundsROOToutputDir = sys.argv[2]
backgroundsROOTdestination = sys.argv[3]


#open and parse list file
with open(backgroundsList) as f:
    lines = f.read().splitlines()

backgrounds = []
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    background_fullpath = line.split()[0].strip()  #first entry in row is background's full path
    backgrounds.append(background_fullpath.split("/")[1])  #we grab just the name

print backgrounds
#run over backgrounds

for background in backgrounds:
    ahaddOut = backgroundsROOTdestination+"/"+background+".root"
    removePrevious = "rm "+ahaddOut
    print removePrevious
    subprocess.call(removePrevious, shell=True)
    ahaddCommand = "ahadd.py "+ahaddOut+" "+backgroundsROOToutputDir+"/"+background+"/"+"*/*/*/*.root"
    print ahaddCommand
    subprocess.call(ahaddCommand, shell=True)   
