from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'BoostedWRAnalysis_ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8'
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = './background_ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8_WR_M-2400_LNu_M-240_cfg.py'

config.Data.inputDataset = '/ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM'
config.Data.splitting = 'Automatic'
config.Data.totalUnits = 10000000
config.Data.publication = True
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'CRAB3_BoostedWRAnalysis'

# These values only make sense for processing data
#    Select input data based on a lumi mask
#config.Data.lumiMask = 'Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt'
#    Select input data based on run-ranges
#config.Data.runRange = '190456-194076'

# Where the output files will be transmitted to
config.Site.storageSite = 'T2_CH_CERN'
