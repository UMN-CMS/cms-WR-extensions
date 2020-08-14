import subprocess as sp

fileList = "resElEl"
years = ['2016', '2017', '2018']

haddcommand = "hadd -f "+fileList+"_all.root "

for year in years:
   command = "cmsRun python/ConfFile_cfg_regression_IDs_LastBinStudy.py inputFiles_load=rootFiles/"+fileList+year+".txt outputFile=out_"+fileList+"_"+year+".root >& out_"+fileList+"_"+year+".txt era="+year
   print command
   sp.call(command, shell=True)
   haddcommand = haddcommand + "out_"+fileList+"_"+year+".root "

print haddcommand
sp.call(haddcommand, shell=True)
