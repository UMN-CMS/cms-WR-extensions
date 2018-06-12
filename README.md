# cms-WR-extensions

SETUP INSTRUCTIONS:

Parts of this code depends on a few different packages:

-BadGlobalMuonTagger package which can be sourced here:
https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2786.html

-HEEPv7 ID information for which is here:
https://twiki.cern.ch/twiki/bin/viewauth/CMS/HEEPElectronIdentificationRun2

As of 12-6-18 the sequence is:
```
cmsrel CMSSW_8_0_25
cd CMSSW_8_0_25/src/
cmsenv
git cms-merge-topic gpetruc:badMuonFilters_80X_v2               #bad global muon tagger
git cms-merge-topic Sam-Harper:HEEPV70VID_8010_ReducedCheckout  #brings in HEEP V70 into VID
mkdir -p ../external/slc6_amd64_gcc530/data/RecoEgamma/ElectronIdentification/ 
  #we need this for the mva weights which runs in VID regardless if you need it or not
git clone git@github.com:cms-data/RecoEgamma-ElectronIdentification ../external/slc6_amd64_gcc530/data/RecoEgamma/ElectronIdentification/data 
  #we need this for the mva weights which runs in VID regardless if you need it or not
git clone git@github.com:Sam-Harper/HEEP.git 
cd HEEP
git checkout HEEPV70  #this is currently the default branch for now but may change in the future
cd ..
mkdir ExoAnalysis
cd ExoAnalysis/
git clone git@github.com:UMN-CMS/cmsWRextensions.git
cd ..
scram b -j 4
```

Example incantations to drive the crab3 interface python (in progress):
```
MC
python createAndSubmitJobsWithCrab3.py -d runBackgroundMC -i Inputs_RunIISpring15DR74_MC/InputList_DYJetsToLL_MG.txt -c Leptoquarks/RootTupleMakerV2/test/rootTupleMaker_MiniAOD_MC_74X_cfg.py

DATA
python createAndSubmitJobsWithCrab3.py -i Inputs_RunII_Data/InputList_Run2015_SingleElectron.txt -d testRunIISingleElectron -c Leptoquarks/RootTupleMakerV2/test/rootTupleMaker_MiniAOD_Data_74X_cfg.py -j  /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-257599_13TeV_PromptReco_Collisions15_25ns_JSON.txt
```
