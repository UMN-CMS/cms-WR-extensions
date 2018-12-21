import sys
import os
import subprocess
from shutil import copyfile

def getFilesRecursively(dir,searchstring,additionalstring = None, skipString = None):

    # thesearchstring = "_"+searchstring+"_"
    thesearchstring = searchstring

    theadditionalstring = None
    if not additionalstring == None:
        theadditionalstring = additionalstring

    cfiles = []
    badfiles = []
    files = []
    print 'ls %s/%s > tmp_0.txt'%(dir,thesearchstring)
    os.system('ls %s/%s > tmp_0.txt'%(dir,thesearchstring))
    tmpFile = open("tmp_4.txt","w")
    with open("tmp_0.txt", 'r') as mylist:
	filesDir = [myfile.replace('\n', '') for myfile in mylist.readlines()]
    print "filesDir: ", filesDir
    for ifileDir, Dir in enumerate(filesDir):
	print 'ls %s/%s/%s > tmp_1.txt'%(dir,thesearchstring,Dir)
        os.system('ls %s/%s/%s > tmp_1.txt'%(dir,thesearchstring,Dir))
	with open("tmp_1.txt", 'r') as mylist:
            filesDir2 = [myfile.replace('\n', '') for myfile in mylist.readlines()]
    	    print "filesDir2: ", filesDir2
	for ifileDir2, Dir2 in enumerate(filesDir2):
	    print 'ls %s/%s/%s/%s > tmp_2.txt'%(dir,thesearchstring,Dir,Dir2)
            os.system('ls %s/%s/%s/%s > tmp_2.txt'%(dir,thesearchstring,Dir,Dir2))
	with open("tmp_2.txt", 'r') as mylist:
            filesDir3 = [myfile.replace('\n', '') for myfile in mylist.readlines()]
            print "filesDir3: ", filesDir3
        for ifileDir3, Dir3 in enumerate(filesDir3):
            print 'ls %s/%s/%s/%s/%s > tmp_3.txt'%(dir,thesearchstring,Dir,Dir2,Dir3)
            os.system('ls %s/%s/%s/%s/%s > tmp_3.txt'%(dir,thesearchstring,Dir,Dir2,Dir3))
            with open("tmp_3.txt", 'r') as mylist:
            	files1 = [myfile.replace('\n', '') for myfile in mylist.readlines()]
	    for ifile2, file2 in enumerate(files1):
		print '%s/%s/%s'%(Dir,Dir2,Dir3)
		print 'file2: ', file2
		tmpFile.write('%s/%s/%s/%s\n'%(Dir,Dir2,Dir3,file2))

    tmpFile.close()


    with open("tmp_4.txt", 'r') as mylist:
        files = [(myfile.replace('\n', ''), True) for myfile in mylist.readlines()]

    nfiles = len(files)
    for ifile, fi in enumerate(files):
        if ifile%100==0:
            print '%i/%i files checked in %s'%(ifile,nfiles,dir+'/'+thesearchstring)
        try:
	    print 'root://cmseos.fnal.gov/%s/%s%s'%(dir[10:],thesearchstring,fi[0])
            filename = 'root://cmseos.fnal.gov/%s/%s%s'%(dir[10:],thesearchstring,fi[0])
            f = ROOT.TFile.Open(filename)
            if f.IsZombie():
                print 'file is zombie'
                f.Close()
                badfiles.append(filename)
                continue
#            elif not f.Get('Events'):
#                print 'tree is false'
#                f.Close()
#                badfiles.append(filename)
#                continue
#            elif not f.Get('Events').InheritsFrom('TTree'):
#                print 'tree is not a tree'
#                f.Close()
#                badfiles.append(filename)
#                continue
            else:
                f.Close()
                cfiles.append(filename)
        except:
            print 'could not open file or tree'
            badfiles.append(filename)
            continue

    return cfiles, badfiles


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
    print "first line split: ", background_fullpath.split("/")[1]
    backgrounds.append(background_fullpath.split("/")[1])  #we grab just the name

print backgrounds
#run over backgrounds

for background in backgrounds:
    ahaddOut = backgroundsROOTdestination+"/"+background+".root"
    removePrevious = "rm "+ahaddOut
    print removePrevious
    subprocess.call(removePrevious, shell=True)
#    filesToConvert, badFiles = getFilesRecursively(backgroundsROOToutputDir,background+'/',None,None)
#    print "filesToConvert: ", filesToConvert
#    print "badFiles: ", badFiles
#    ahaddCommand = "python ahadd.py "+ahaddOut+" %s\n"%((' '.join(badFiles[0:len(badFiles)])))
#    haddCommand = "hadd "+ahaddOut+" %s\n"%((' '.join(badFiles[0:len(badFiles)])))
#    print "ahaddCommand: ", ahaddCommand
#    subprocess.call(ahaddCommand, shell=True)
#    ahaddCommand = "hadd "+ahaddOut+" "+ "`xrdfsls -u " + backgroundsROOToutputDir +"/"+ background + " | grep '\.root'`"
    ahaddCommand = "python ahadd.py "+ahaddOut+" "+backgroundsROOToutputDir+"/"+background+"/"+"*/*/*/*.root"
    print ahaddCommand
    subprocess.call(ahaddCommand, shell=True)   
