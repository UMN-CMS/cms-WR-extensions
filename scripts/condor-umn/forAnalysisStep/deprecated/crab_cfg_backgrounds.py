from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'BoostedWRAnalysis_CHANNEL'
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = 'PYTHONCFG'

config.Data.inputDataset = 'DATASET'
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
