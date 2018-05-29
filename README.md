# cms-WR-extensions

SETUP INSTRUCTIONS:

Parts of this code depend on the BadGlobalMuonTagger package which can be sourced here:
https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2786.html

Pulling the code out of there and cloning this repo is enough to get started

As of 21-6-17 the sequence is:
```
cmsrel CMSSW_8_0_25
cd CMSSW_8_0_25/src/
cmsenv
git cms-merge-topic gpetruc:badMuonFilters_80X_v2
mkdir ExoAnalysis
cd ExoAnalysis/
git clone git@github.com:UMN-CMS/cms-WR-extensions.git
cd ..
scram b -j 4
```

Example incantations to drive the crab3 interface python:
```
MC
python createAndSubmitJobsWithCrab3.py -d runBackgroundMC -i Inputs_RunIISpring15DR74_MC/InputList_DYJetsToLL_MG.txt -c Leptoquarks/RootTupleMakerV2/test/rootTupleMaker_MiniAOD_MC_74X_cfg.py

DATA
python createAndSubmitJobsWithCrab3.py -i Inputs_RunII_Data/InputList_Run2015_SingleElectron.txt -d testRunIISingleElectron -c Leptoquarks/RootTupleMakerV2/test/rootTupleMaker_MiniAOD_Data_74X_cfg.py -j  /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-257599_13TeV_PromptReco_Collisions15_25ns_JSON.txt
```
