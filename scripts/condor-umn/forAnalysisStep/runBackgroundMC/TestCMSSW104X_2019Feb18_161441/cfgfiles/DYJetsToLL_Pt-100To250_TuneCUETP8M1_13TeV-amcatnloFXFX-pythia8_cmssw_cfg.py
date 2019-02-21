import FWCore.ParameterSet.Config as cms
import HLTrigger.HLTfilters.hltHighLevel_cfi
from FWCore.ParameterSet.VarParsing import VarParsing
import os
#INPUT PARSING SECTION
options = VarParsing ('analysis')

options.register ('gitTag',
				  'applePie',
				  VarParsing.multiplicity.singleton,
				  VarParsing.varType.string,
				  "Git Tag")

options.register( 'isMC',
				  True,
				  VarParsing.multiplicity.singleton,
				  VarParsing.varType.bool,
				  "True if is MC dataset")

options.register( 'ISmcatnlo',
                  False,
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.bool,
                  "True if is MC @ NLO dataset")

options.register( 'doFast',
                  False,
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.bool,
                  "True to run a condensed analysis for HiggsCombine"
               )

options.register( 'reco',
                  '80X',
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  "MC or data CMSSW reco")

options.register( 'era',
                  '2016',
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  "Year of Run II")
options.parseArguments()

#LOCAL VARIABLE DEFINITIONS
muonID =' userInt("highPtID") == 1'

process = cms.Process("Analysis")

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
#setup global tag
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
from Configuration.AlCa.autoCond import autoCond

process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_TrancheIV_v8') #
if not options.isMC: process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016SeptRepro_v7')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source ("PoolSource",
	  fileNames = cms.untracked.vstring (options.inputFiles),
# 	  skipEvents = cms.untracked.uint32(88850)
)

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')
)
process.options.allowUnscheduled = cms.untracked.bool(False)

process.TFileService = cms.Service("TFileService", 
                        fileName = cms.string(options.outputFile)
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
#HERE WE RUN A MODULE FROM SAM HARPER WHICH INSERTS HEEP CUT INFO INTO THE PAT ELECTRON USER DATA
#we setup the HEEP ID V7.0 and enable VID via the following function
#and then add it to a new collection of pat::Electrons
#there is the option to call the new collection "slimmedElectrons" (useStdName=True)
#otherwise it calls them "heepElectrons"
#it creates a sequence "process.heepSequence" which we add to our path

#from HEEP.VID.tools import addHEEPV70ElesMiniAOD

from ExoAnalysis.cmsWRextensions.tools import addHEEPV70ElesMiniAOD
addHEEPV70ElesMiniAOD(process,useStdName=False)

from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox
jetToolbox( process, 'ak8', 'jetSequence', 'out', PUMethod='Puppi', miniAOD=True, runOnMC=options.isMC, addSoftDrop=True , addSoftDropSubjets=True, addNsub=True, JETCorrPayload='AK8PFPuppi', JETCorrLevels=['L1FastJet','L2Relative', 'L3Absolute'])  

process.options.allowUnscheduled = cms.untracked.bool(True)
##this is our example analysis module reading the results, you will have your own module
#process.heepIdExample = cms.EDAnalyzer("HEEPV70PATExample",
#                                       eles=cms.InputTag("slimmedElectrons"),
#                                       )

#process.p = cms.Path(
#    process.heepSequence*
#    process.heepIdExample) #our analysing example module, replace with your module
#

process.muonSelectionSeq = cms.Sequence(cms.ignore(process.badGlobalMuonTagger) * cms.ignore(process.cloneGlobalMuonTagger) * process.removeBadAndCloneGlobalMuons * process.tunePMuons * process.tuneIDMuons)

muonPaths = cms.vstring("HLT_Mu50_v", "HLT_TkMu50_v")
if options.era == '2016':
	muonPaths = cms.vstring("HLT_Mu50_v", "HLT_TkMu50_v")
elif options.era == '2017':
	muonPaths = cms.vstring("HLT_Mu50_v")
elif options.era == '2018':
	muonPaths = cms.vstring("HLT_Mu50", "HLT_OldMu100", "HLT_TkMu100")

process.analysis = cms.EDAnalyzer('cmsWRextension',
                              genJets = cms.InputTag("slimmedGenJets"),
                              AK8genJets = cms.InputTag("slimmedGenJetsAK8"),
                              genParticles = cms.InputTag("prunedGenParticles"),
                              highMuons = cms.InputTag("tuneIDMuons"),
                              highElectrons = cms.InputTag("heepElectrons"),
                              electrons2 = cms.InputTag("selectedElectrons"),
                              #electrons2 = cms.InputTag("heepElectrons"),
                              eleFullInfoMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose"),
                              regMuons = cms.InputTag("removeBadAndCloneGlobalMuons"),
                              recoJets = cms.InputTag("slimmedJets"),
                              AK4recoCHSJets = cms.InputTag("slimmedJets"),
                              AK8recoCHSJets = cms.InputTag("slimmedJetsAK8"),
                              AK8recoPUPPIJets = cms.InputTag("selectedPatJetsAK8PFPuppi"),
                              subJetName         = cms.InputTag('selectedPatJetsAK8PFPuppiSoftDropPacked'),
                              #AK8recoPUPPIJets = cms.InputTag("AK8PFJetsPuppi"),
          		              jettinessPUPPI  = cms.untracked.string("NjettinessAK8Puppi"),
			                  jecUncName  = (cms.untracked.string('AK8Puppi')),
			                  rhoLabel    = cms.InputTag("fixedGridRhoFastjetAll"),
                              vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
			                  edmPileupInfo = cms.InputTag('slimmedAddPileupInfo'),
                              genInfo = cms.InputTag("generator"),
                              met = cms.InputTag("slimmedMETsPuppi"),
                              trigResults = cms.InputTag("TriggerResults","","HLT"),
                              trigObjs = cms.InputTag("selectedPatTrigger"),
                              muonPathsToPass = muonPaths,
                              muonFiltersToPass = cms.vstring(""),
			                  #electronPathsToPass = cms.vstring("HLT_Ele115_CaloIdVT_GsfTrkIdT_v", "HLT_Photon175_v"),
			                  electronPathsToPass = cms.vstring("HLT_Ele27_WPTight_Gsf_v", "HLT_Ele115_CaloIdVT_GsfTrkIdT_v", "HLT_Photon175_v"),
			                  electronFiltersToPass = cms.vstring(""),
                              doTrig = cms.untracked.bool(True),
                              wantHardProcessMuons = cms.untracked.bool(True),
                              doGen = cms.untracked.bool(True),
                              isSignal = cms.untracked.bool(False),
                              doFast = cms.untracked.bool(options.doFast),
                              isMC = cms.untracked.bool(options.isMC),
			                  amcatnlo = cms.untracked.bool(options.ISmcatnlo),
                              #MCL = cms.untracked.double(100),
                              #MCU = cms.untracked.double(8000),
                              outputTag = cms.untracked.string(options.gitTag),
							  era = cms.untracked.string(options.era)

)

process.load('FWCore.Modules.printContent_cfi')

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
	
process.selectedElectrons = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("slimmedElectrons"),
    cut = cms.string("pt>5 && abs(eta)")
)
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('selectedElectrons')
process.heepIDVarValueMaps.elesMiniAOD  = 'selectedElectrons'
process.electronMVAValueMapProducer.srcMiniAOD = cms.InputTag('selectedElectrons')
process.heepElectrons.src = cms.InputTag('selectedElectrons')

process.totalPath = cms.Path(process.selectedElectrons * process.heepSequence
                           * process.muonSelectionSeq * process.analysis)

process.TFileService.fileName = "DYJetsToLL_Pt-100To250.root"

process.GlobalTag.globaltag = "94X_mcRun2_asymptotic_v3"
