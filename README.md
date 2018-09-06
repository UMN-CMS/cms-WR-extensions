# cms-WR-extensions

SETUP INSTRUCTIONS:

Parts of this code depends on a few different packages:

-BadGlobalMuonTagger package which can be sourced here:
https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2786.html

-HEEPv7 ID information for which is here:
https://twiki.cern.ch/twiki/bin/viewauth/CMS/HEEPElectronIdentificationRun2

As of 12-6-18 the sequence is:
```
cmsrel CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src/
cmsenv
git cms-merge-topic cms-egamma:EGM_gain_v1
cd EgammaAnalysis/ElectronTools/data
git clone -b Moriond17_gainSwitch_unc https://github.com/ECALELFS/ScalesSmearings.git
cd ../../..
git cms-merge-topic rafaellopesdesa:RegressionCheckNegEnergy
git cms-merge-topic gpetruc:badMuonFilters_80X_v2               #bad global muon tagger
git cms-merge-topic Sam-Harper:HEEPV70VID_8010_ReducedCheckout  #brings in HEEP V70 into VID
git cms-merge-topic ikrav:egm_id_80X_v3 #for other E/gamma IDs in VID if you wish to have them
git cms-merge-topic Sam-Harper:PackedCandNoPuppi #only necessary to run HEEP V70 on AOD (it will crash if this is not present looking for puppi candidates
mkdir -p ../external/slc6_amd64_gcc530/data/RecoEgamma/ElectronIdentification/ 
  #we need this for the mva weights which runs in VID regardless if you need it or not
git clone git@github.com:cms-data/RecoEgamma-ElectronIdentification ../external/slc6_amd64_gcc530/data/RecoEgamma/ElectronIdentification/data 
  #we need this for the mva weights which runs in VID regardless if you need it or not
git clone git@github.com:Sam-Harper/HEEP.git 
cd HEEP
git checkout HEEPV70  #this is currently the default branch for now but may change in the future
cd ..
git cms-merge-topic Sam-Harper:NewEGModifiers_8010
mkdir ExoAnalysis
cd ExoAnalysis/
git clone git@github.com:UMN-CMS/cmsWRextensions.git
cd ..
scram b -j 4

git clone git@github.com:cms-jet/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_80X_V3
scram b -j 18

git clone https://github.com/Michael-Krohn/BaconAna.git
scram b -j8
```

Example incantations to drive the crab3 interface python (in progress):
```
MC
python createAndSubmitJobsWithCrab3.py -d runBackgroundMC -i ../../../samples/backgrounds/fullBackgroundDatasetList_no_ext_noDiBoson.txt -c ExoAnalysis/cmsWRextensions/python/ConfFile_cfg_regression.py

DATA
python createAndSubmitJobsWithCrab3.py -d runBackgroundData -i ../../../samples/data/data_datasets_SingleMuon.txt -c ../../../python/ConfFile_cfg_regression.py -v Electron_Regression
```
Example incantations to run the analysis by hand:
```
cmsRun python/ConfFile_cfg.py inputFiles_load=samples/backgrounds/WJetsToLNu_Pt-100To250_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8.txt outputFile=out.root >& out.txt &

cmsRun python/ConfFile_cfg.py inputFiles=root://cms-xrd-global.cern.ch///store/mc/RunIISummer16MiniAODv2/WJetsToLNu_Pt-250To400_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/AC65E566-52D0-E611-ACAA-1866DAEA812C.root outputFile=out.root >& out.txt &
```

How to copy files to CERNbox for sharing.

This will simply deal with copy files from the LPC to CERNbox.  It is also possible to fuse mount a CERNbox space on any computer.

At LPC (assuming you have a CERN computing account)
```
kinit <username>@CERN.CH
xrdcp /local_path_to_file/potatoe.txt root://eosuser.cern.ch//eos/user/<u>/<username>/path_to_file/potatoe.txt
```
NOTE: CERN box is in a different eos from the cms project space.
  
These can be then viewed here:
https://cernbox.cern.ch/index.php/apps/files/?dir=/&

---SOON DEPRECATED IN FAVOR OF CERNBOX----
REPO For Keeping Plots Organized:

https://gitlab.cern.ch/mkrohn/BoostedWR

-To clone: 
```
git clone ssh://git@gitlab.cern.ch:7999/mkrohn/BoostedWR.git
```
