# cms-WR-extensions
## 2016 (As of 12-6-18)
SETUP INSTRUCTIONS:

Parts of this code depends on a few different packages:

-BadGlobalMuonTagger package which can be sourced here:
https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2786.html

-HEEPv7 ID information for which is here:
https://twiki.cern.ch/twiki/bin/viewauth/CMS/HEEPElectronIdentificationRun2

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

git clone git@github.com:Michael-Krohn/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_102X
git clone https://github.com/Michael-Krohn/BaconAna.git

scram b -j32
```

Example incantations to drive the crab3 interface python (in progress):
```
MC
python createAndSubmitJobsWithCrab3.py -d runBackgroundMC -i ../../../samples/backgrounds/fullBackgroundDatasetList_no_ext_noDiBoson.txt -c ExoAnalysis/cmsWRextensions/python/ConfFile_cfg_regression_IDs.py

2017 Signal Samples:
python createAndSubmitJobsWithCrab3.py -i ../../../samples/signals/WRtoNLtoLLJJ_2017/WRtoNLtoLLJJ.txt -v 2017Signal -d runBackgroundMC -c ../../../python/ConfFile_cfg_regression_IDs.py -f True

DATA
python createAndSubmitJobsWithCrab3.py -d runBackgroundData -i ../../../samples/data/data_datasets_SingleMuon.txt -c ../../../python/ConfFile_cfg_regression_IDs.py -v Electron_Regression
```
Example incantations to run the analysis by hand:
```
cmsRun python/ConfFile_cfg_regression_IDs.py inputFiles=root://cmsxrootd.fnal.gov//store/user/suoh/WRtoNLtoLLJJ_WR7000_N2800/CMSSW_9_4_6_patch1__MINIAOD/190418_080016/0000/MINIAOD_28.root doFast=True isSignal=True isMC=True checkZ=False >& out.txt &

```

## Legacy Run II (**IN PROGRESS** as of 1-22-2019)

SETUP INSTRUCTIONS
```
cmsrel CMSSW_10_4_0_patch1
cd CMSSW_10_4_0_patch1/src/
cmsenv

git cms-init

git clone git@github.com:Sam-Harper/HEEP.git 
cd HEEP
git checkout HEEPV70  #this is currently the default branch for now but may change in the future
cd ..

git cms-merge-topic lathomas:L1Prefiring_M

mkdir ExoAnalysis
cd ExoAnalysis/
git clone git@github.com:UMN-CMS/cmsWRextensions.git -b tweaksFor104X
cd ..

git clone git@github.com:Michael-Krohn/JetToolbox.git JMEAnalysis/JetToolbox -b jetToolbox_102X
git clone https://github.com/Michael-Krohn/BaconAna.git

scram b -j32
```

## Other Recipes

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

Steps for running Combine:

1.Set doFast to true. This can be done by setting -f True for the createAndSubmitJobsWithCrab3.py script.

2.Make the workspaces for fitting(ttbar SF is hard-coded within these scripts):
```
python makeWorkspaces_Resolved.py ../../../samples/allSamples2016.txt ../../../Output/Full2016_3rd/ ../../../Output/Full2016_3rd/Workspace/

python makeWorkspaces_Boosted.py ../../../samples/allSamples2016.txt ../../../Output/Full2016_3rd/ ../../../Output/Full2016_3rd/Workspace/
```
3.Do the fits and make the workspaces for combione:
```
python dualFit_makeWorkspaces.py True

python dualFit_makeWorkspaces.py False
```

3.Make the datacards:
```
python makeCardsMuMuJJ_Resolved.py ../../../Output/Full2016_3rd/Workspace/

python makeCardsMuMuJJ_Boosted_fit.py ../../../Output/Full2016_3rd/Workspace/
```
