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
git cms-merge-topic gpetruc:badMuonFilters_80X
mkdir ExoAnalysis
cd ExoAnalysis/
git clone git@github.com:UMN-CMS/cms-WR-extensions.git
cd ..
scram b -j 4
```
