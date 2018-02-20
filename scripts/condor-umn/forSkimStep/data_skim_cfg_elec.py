import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList

process = cms.Process("Demo")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source ("PoolSource",
	  fileNames = cms.untracked.vstring ("file:YOURROOTFILEHERE.root"),
)
process.source.lumisToProcess = LumiList.LumiList(filename = '/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/samples/data/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt').getVLuminosityBlockRange()

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.TFileService = cms.Service("TFileService", 
                        fileName = cms.string('file:COOLESTPLOTSYAEVERSEEN')
)   #for MC

process.skim = cms.EDFilter('skimEventsForElectrons',
                              recoElectrons = cms.InputTag("slimmedElectrons"),
                              AK8recoJets = cms.InputTag("slimmedJetsAK8"),
                              genInfo = cms.InputTag("generator"),
                              met = cms.InputTag("slimmedMETs"),
                              isMC = cms.untracked.bool(False)
)
process.skimOutput = cms.OutputModule("PoolOutputModule",
        dataset = cms.untracked.PSet(
            dataTier = cms.untracked.string('MINIAOD'),
            filterName = cms.untracked.string('wrSkim')
        ),
        fileName = cms.untracked.string('file:skimmedOut.root'),

        SelectEvents = cms.untracked.PSet(
            SelectEvents = cms.vstring('p')

        )
)
 

process.p = cms.Path(process.skim)
process.out = cms.EndPath(process.skimOutput)
process.schedule = cms.Schedule(process.p,process.out)
