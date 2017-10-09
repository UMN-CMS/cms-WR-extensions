import os
import sys
import subprocess


WrMasses=[800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000, 800, 1600, 2400, 3200, 4000, 6000]
NuMasses=[ 80,  160,  240,  320,  400,  600, 160,  320,  480,  640,  800, 1200, 233,  533,  800, 1067, 1333, 2000]
DataSetsDirectory="/data/whybee0b/user/aevans/"
nEvents = -1

for massPoint in range(0, (len(WrMasses)-1)) : 
    MWRUCUT = WrMasses[massPoint]*1.3
    MWRLCUT = WrMasses[massPoint]*0.7
    parseFiles = "sed 's/MASSWR/"+str(WrMasses[massPoint])+"/g ; s/MASSNU/"+str(NuMasses[massPoint])+"/g ; s/N_EVENTS/"+str(nEvents)+"/g ; s/MWRUCUT/"+str(MWRUCUT)+"/g ; s/MWRLCUT/"+str(MWRLCUT)+"/g' WR_M-UNDEF_ToLNu_M-UNDEF_Analysis_MuMuJJ_cfg.py > WR_M-"+str(WrMasses[massPoint])+"_ToLNu_M-"+str(NuMasses[massPoint])+"_Analysis_MuMuJJ_cfg.py" 
    lsROOTfiles = "ls "+str(DataSetsDirectory)+"/WR_M-"+str(WrMasses[massPoint])+"_ToLNu_M-"+str(NuMasses[massPoint])+"_MiniAOD_MuMuJJ/*000.root > WR_M-"+str(WrMasses[massPoint])+"_ToLNu_M-"+str(NuMasses[massPoint])+"_MiniAOD_MuMuJJ.txt"
    
    runCondor = "condor_filelist.perl --batch 1 --nosubmit WR_M-"+str(WrMasses[massPoint])+"_ToLNu_M-"+str(NuMasses[massPoint])+"_Analysis_MuMuJJ_cfg.py WR_M-"+str(WrMasses[massPoint])+"_ToLNu_M-"+str(NuMasses[massPoint])+"_MiniAOD_MuMuJJ.txt --memreq 2500"
    
    subprocess.call(parseFiles,shell=True)
    subprocess.call(lsROOTfiles,shell=True)
    subprocess.call(runCondor,shell=True)
