import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing ('analysis')

options.maxEvents = -1
options.parseArguments()
muonID =' userInt("highPtID") == 1'

process = cms.Process("Analyze")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source ("PoolSource",
	  fileNames = cms.untracked.vstring ("file:WR_4000_ToLNu_MASSNR_MiniAOD_MuMuJJ.root")
      ,
)
process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)
process.TFileService = cms.Service("TFileService", 
    fileName = cms.string("WR_4000_ToLNu_1333_Analysis_MuMuJJ.root")

    )
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

process.muonSelectionSeq = cms.Sequence(process.badGlobalMuonTagger * process.cloneGlobalMuonTagger * process.removeBadAndCloneGlobalMuons * process.tunePMuons * process.tuneIDMuons)




process.analyze = cms.EDAnalyzer('cmsWRextension',
                              genJets = cms.InputTag("slimmedGenJets"),
                              AK8genJets = cms.InputTag("slimmedGenJetsAK8"),
                              genParticles = cms.InputTag("prunedGenParticles"),
                              regMuons = cms.InputTag("removeBadAndCloneGlobalMuons"),
                              highLeptons = cms.InputTag("tuneIDMuons"),
                              recoJets = cms.InputTag("slimmedJets"),
                              AK8recoJets = cms.InputTag("slimmedJetsAK8"),
                              vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
                              genInfo = cms.InputTag("generator"),
                              met = cms.InputTag("slimmedMETsPuppi"),
                              wantHardProcessMuons = cms.untracked.bool(True),
                              doGen = cms.untracked.bool(True),
                              isMC = cms.untracked.bool(True),
                              MCL = cms.untracked.double(2800.0),
                              MCU = cms.untracked.double(5200.0),
                              flavorSideband = cms.untracked.bool(False)
)

process.p = cms.Path(process.muonSelectionSeq * process.analyze)

