import os

massgrids = open("WRtoNLtoLLJJ.txt").readlines()
for massgrid in massgrids:
  this_sample = massgrid.strip()
  xrootdpath = "/xrootd_user/shjeon/xrootd/2018Run_WR_Dilep/CRAB/"+this_sample+"/CMSSW_10_2_11__MINIAOD/"
  os.system("ls -1 "+xrootdpath+" &> temp.txt")
  miniaods = open("temp.txt").readlines()
  newfile = open(this_sample+".txt","wt")
  for miniaod in miniaods:
    this_miniaod = miniaod.strip()
    newfile.write(this_sample+"\t/xrd/store/user/shjeon/2018Run_WR_Dilep/CRAB/"+this_sample+"/CMSSW_10_2_11__MINIAOD/"+this_miniaod+"\n")

