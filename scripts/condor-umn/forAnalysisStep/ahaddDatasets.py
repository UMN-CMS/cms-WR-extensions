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
    print "python ahadddatasets.py ../../../samples/datasets/multiCrabTest.txt /eos/cms/store/user/aevans/undefined_2018Jun15_111107 /afs/cern.ch/work/a/aevans/public/thesis/datasetStacks/"
    print ""
    exit(0)
if len(sys.argv) != 4:
    print "inputs not understood, try --help/-h"
    exit(1)
#proceed with input parsing

datasetsList = sys.argv[1]
datasetsROOToutputDir = sys.argv[2]
datasetsROOTdestination = sys.argv[3]


#open and parse list file
with open(datasetsList) as f:
    lines = f.read().splitlines()

datasets = []
lineNum = 0
for line in lines:
    if lineNum < 2 : 
        lineNum+=1
        continue
    dataset_fullpath = line.split()[0].strip()  #first entry in row is dataset's full path
    datasetPrefix = dataset_fullpath.split("/")[1]
    if ("SingleElectron" in datasetPrefix or "SingleMuon" in datasetPrefix):
        datasetFlavor = datasetPrefix + "--" + dataset_fullpath.split("/")[2]
        datasets.append(datasetFlavor)
    else:
        datasets.append(datasetPrefix)  #we grab just the name BUT this is a bad idea with data as all the like datasets have the same name

print datasets
#run over datasets

for dataset in datasets:
    ahaddOut = datasetsROOTdestination+"/"+dataset+".root"
    removePrevious = "rm "+ahaddOut
    print removePrevious
    subprocess.call(removePrevious, shell=True)
    if("SingleElectron" in dataset or "SingleMuon" in dataset): #DATA
        ahaddCommand = "ahadd.py "+ahaddOut+" "+datasetsROOToutputDir+"/"+dataset.split("--")[0]+"/"+dataset.split("--")[1]+"*/*/*/*.root"
    else:
        ahaddCommand = "ahadd.py "+ahaddOut+" "+datasetsROOToutputDir+"/"+dataset+"/"+"*/*/*/*.root"
    print ahaddCommand
    subprocess.call(ahaddCommand, shell=True)   
