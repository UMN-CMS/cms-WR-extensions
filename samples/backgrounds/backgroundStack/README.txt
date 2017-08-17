THIS STORES THE STACK OF BACKGROUNDS CURRENTLY RUN ON AND THE RESPECTIVE CROSS-SECTIONS EITHER FOUND IN THE TWIKIS OR CALCULATED WITH ANA.PY:

cmsrel CMSSW_7_5_0

cd CMSSW_7_5_0/src

cmsenv

curl https://raw.githubusercontent.com/syuvivida/generator/master/cross_section/runJob/ana.py  -o ana.py

cmsRun ana.py inputFiles_load="xxx.root" maxEvents=-1

https://twiki.cern.ch/twiki/bin/viewauth/CMS/HowToGenXSecAnalyzer
