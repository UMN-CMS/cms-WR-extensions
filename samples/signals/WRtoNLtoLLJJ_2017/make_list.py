import os

massgrids = open("WRtoNLtoLLJJ.txt").readlines()
for massgrid in massgrids:
  this_sample = massgrid.strip()
  xrootdpath = "/xrootd_user/shjeon/xrootd/2017Run_WR_Dilep/CRAB/"+this_sample+"/CMSSW_9_4_12__MINIAOD/"
  os.system("ls -1 "+xrootdpath+" &> temp.txt")
  miniaods = open("temp.txt").readlines()
  newfile = open(this_sample+".txt","wt")
  for miniaod in miniaods:
    this_miniaod = miniaod.strip()
    newfile.write(this_sample+"\t/xrd/store/user/shjeon/2017Run_WR_Dilep/CRAB/"+this_sample+"/CMSSW_9_4_12__MINIAOD/"+this_miniaod+"\n")

