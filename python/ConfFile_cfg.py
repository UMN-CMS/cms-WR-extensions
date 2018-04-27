import FWCore.ParameterSet.Config as cms
import HLTrigger.HLTfilters.hltHighLevel_cfi
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing ('analysis')

options.maxEvents = -1
options.parseArguments()
muonID =' userInt("highPtID") == 1'

process = cms.Process("Analysis")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source ("PoolSource",
	  fileNames = cms.untracked.vstring (options.inputFiles),
)

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)

process.TFileService = cms.Service("TFileService", 
                        fileName = cms.string(options.outputFile)
)  

#process.TriggerFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone(
#    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
#    #HLTPaths = ['HLT_Mu9', 'HLT_Mu15_v*'],
#    #HLTPaths = ['HLT_IsoMu17_v*'],
#    HLTPaths = ['HLT_Mu50_v*', 'HLT_TkMu50_v*'], #  # provide list of HLT paths (or patterns) you want
#    #HLTPaths = ['@'],
#    andOr = cms.bool(True),   # how to deal with multiple triggers: True (OR) accept if ANY is true, False (AND) accept if ALL are true
#    throw = False, ## throw exception on unknown path names
#)




process.badGlobalMuonTagger = cms.EDFilter("BadGlobalMuonTagger",
    muons = cms.InputTag("slimmedMuons"),
    vtx   = cms.InputTag("offlineSlimmedPrimaryVertices"),
    muonPtCut = cms.double(20),
    selectClones = cms.bool(False),
    taggingMode = cms.bool(True),
    verbose     = cms.untracked.bool(False)
)
process.cloneGlobalMuonTagger = process.badGlobalMuonTagger.clone(
    selectClones = cms.bool(True)
)

process.removeBadAndCloneGlobalMuons = cms.EDProducer("MuonRefPruner",
    input = cms.InputTag("slimmedMuons"),
    toremove = cms.InputTag("badGlobalMuonTagger", "bad"),
    toremove2 = cms.InputTag("cloneGlobalMuonTagger", "bad")
)

process.tunePMuons = cms.EDProducer("TunePMuonProducer",
		src = cms.InputTag("removeBadAndCloneGlobalMuons")
		#src = cms.InputTag("slimmedMuons")
)

### muon ID and isolation
# make a collection of TuneP muons which pass isHighPt ID
process.tuneIDMuons = cms.EDFilter("PATMuonSelector",
                               src = cms.InputTag("tunePMuons"),
                               cut = cms.string(muonID),
)
                                   
#process.muonSelectionSeq = cms.Sequence(process.TriggerFilter * process.badGlobalMuonTagger * process.cloneGlobalMuonTagger * process.removeBadAndCloneGlobalMuons * process.tunePMuons * process.tuneIDMuons)
process.muonSelectionSeq = cms.Sequence(cms.ignore(process.badGlobalMuonTagger) * cms.ignore(process.cloneGlobalMuonTagger) * process.removeBadAndCloneGlobalMuons * process.tunePMuons * process.tuneIDMuons)

process.analysis = cms.EDAnalyzer('cmsWRextension',
                              genJets = cms.InputTag("slimmedGenJets"),
                              AK8genJets = cms.InputTag("slimmedGenJetsAK8"),
                              genParticles = cms.InputTag("prunedGenParticles"),
                              highMuons = cms.InputTag("tuneIDMuons"),
                              highElectrons = cms.InputTag("slimmedElectrons"),
                              regMuons = cms.InputTag("removeBadAndCloneGlobalMuons"),
                              recoJets = cms.InputTag("slimmedJets"),
                              AK8recoJets = cms.InputTag("slimmedJetsAK8"),
                              vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
                              genInfo = cms.InputTag("generator"),
                              met = cms.InputTag("slimmedMETsPuppi"),
                              #trigResults = cms.InputTag("TriggerResults","","HLT"),
                              trigObjs = cms.InputTag("selectedPatTrigger"),
                              pathsToPass = cms.vstring("HLT_Mu50_v*", "HLT_TkMu50_v*"),
                              filtersToPass = cms.vstring(""),
                              doTrig = cms.untracked.bool(False),
                              wantHardProcessMuons = cms.untracked.bool(True),
                              doGen = cms.untracked.bool(True),
                              isMC = cms.untracked.bool(True),
                              MCL = cms.untracked.double(100),
                              MCU = cms.untracked.double(8000),
                              flavorSideband = cms.untracked.bool(False)

)

process.p = cms.Path(process.muonSelectionSeq * process.analysis)
