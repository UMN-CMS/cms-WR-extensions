CONDOR JOBS ARE EXECUTED WITH THE COMMAND: condor_filelist.py

An example of which:
condor_filelist.py --batch 1 --prodSpace /data/cmszfs1/user/lesko/aMCatNloScaleUncertaintyOutput/PowhegCombined SMPVJ_MC_cfg.py PowHegCombined.txt

command  --batch (jobs per file) --prodSpace (where to put the data) cmsRunConfigFile.py InputRootFiles.txt

Text files contain root files can be found in the *Samples directories at the
root of the repository

In order for the scripts to run properly, you must use the
condor_prod_mod.perl in place of the usual one
