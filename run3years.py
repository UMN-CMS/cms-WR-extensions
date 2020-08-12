import subprocess as sp

fileList = "resMuMu"
years = ['2016', '2017', '2018']

haddcommand = "hadd -f "+fileList+"_all.root "

for year in years:
   command = "cmsRun python/ConfFile_cfg_regression_IDs_LastBinStudy.py inputFiles_load=rootFiles/"+fileList+".txt outputFile=out_"+fileList+".root >& out_"+fileList+".txt era="+year
   print command
   sp.call(command, shell=True)
   haddcommand = haddcommand + "out_"+fileList+".root "

print haddcommand
sp.call(haddcommand, shell=True)
