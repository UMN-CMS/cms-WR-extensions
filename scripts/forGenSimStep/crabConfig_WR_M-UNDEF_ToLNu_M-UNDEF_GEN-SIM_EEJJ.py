# CRAB3 config template for cmsWR

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.section_("General")
config.General.requestName = 'WR_M-UNDEF_ToLNu_M-UNDEF_GEN-SIM_EEJJ' 
config.General.workArea = 'crab_projects'  
config.General.transferOutputs = True
config.General.transferLogs = True  #False

config.section_("JobType")
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'WR_M-UNDEF_ToLNu_M-UNDEF_GEN-SIM_EEJJ.py'  

config.section_("Data")
config.Data.outputPrimaryDataset = 'WRToEEJJ_MW-UNDEF_MNu-UNDEF_TuneCUETP8M1_13TeV-pythia8'
config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 250 
NJOBS = 200
config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
config.Data.outLFNDirBase = '/store/user/gnegro/cmsWR/' 
config.Data.publication = True
config.Data.outputDatasetTag = 'RunIIWinter16_80X_GEN-SIM' 
# config.Data.publishDBS = 'phys03'  

config.section_("Site")
config.Site.storageSite = "T2_FR_GRIF_IRFU"
config.Site.blacklist = ["T3_US_UCR", "T2_US_Vanderbilt", "T2_EE_Estonia"] 