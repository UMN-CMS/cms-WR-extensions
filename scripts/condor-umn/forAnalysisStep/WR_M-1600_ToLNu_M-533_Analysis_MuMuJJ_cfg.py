import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing ('analysis')

options.maxEvents = -1
options.parseArguments()

process = cms.Process("Demo")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source ("PoolSource",
	  fileNames = cms.untracked.vstring ("file:WR_1600_ToLNu_MASSNR_MiniAOD_MuMuJJ.root")
      ,
)
process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)
process.TFileService = cms.Service("TFileService", 
    fileName = cms.string("WR_1600_ToLNu_533_Analysis_MuMuJJ.root")

    )


process.demo = cms.EDAnalyzer('cmsWRextension',
                              genJets = cms.InputTag("slimmedGenJetsAK8"),
                              genParticles = cms.InputTag("prunedGenParticles")
)


process.p = cms.Path (process.demo)

