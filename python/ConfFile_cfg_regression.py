import FWCore.ParameterSet.Config as cms
import HLTrigger.HLTfilters.hltHighLevel_cfi
from FWCore.ParameterSet.VarParsing import VarParsing
###############################################################################
from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
process = regressionWeights(process)

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
                  calibratedPatElectrons  = cms.PSet( initialSeed = cms.untracked.uint32(8675389),
                                                      engineName = cms.untracked.string('TRandom3'),
                                                      ),
                  calibratedPatPhotons    = cms.PSet( initialSeed = cms.untracked.uint32(8675389),
                                                      engineName = cms.untracked.string('TRandom3'),
                                                      ),
                                                   )
process.load('EgammaAnalysis.ElectronTools.regressionApplication_cff')
process.load('EgammaAnalysis.ElectronTools.calibratedPatElectronsRun2_cfi')
process.load('EgammaAnalysis.ElectronTools.calibratedPatPhotonsRun2_cfi')

# Path and EndPath definitions
process.EGMRegression = cms.Path(process.regressionApplication)
process.EGMSmearerElectrons = cms.Path(process.calibratedPatElectrons)
process.EGMSmearerPhotons   = cms.Path(process.calibratedPatPhotons)

# Set the lines below to True or False depending if you are correcting the scale (data) or smearing the resolution (MC) 
process.EGMSmearerElectrons.isMC = cms.bool(False)
process.EGMSmearerPhotons.isMC = cms.bool(False)

# process.analysis is the path with your analyzer
process.schedule = cms.Schedule(process.EGMRegression,process.EGMSmearerElectrons,process.EGMSmearerPhotons,process.analysis)
################################################################################

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
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:mc', '') #WORKS FOR TESTING BUT NOT TRUSTWORTHY
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_TrancheIV_v8') #



process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

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
#HERE WE RUN A MODULE FROM SAM HARPER WHICH INSERTS HEEP CUT INFO INTO THE PAT ELECTRON USER DATA
#we setup the HEEP ID V7.0 and enable VID via the following function
#and then add it to a new collection of pat::Electrons
#there is the option to call the new collection "slimmedElectrons" (useStdName=True)
#otherwise it calls them "heepElectrons"
#it creates a sequence "process.heepSequence" which we add to our path

from HEEP.VID.tools import addHEEPV70ElesMiniAOD
addHEEPV70ElesMiniAOD(process,useStdName=True)


from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox
jetToolbox( process, 'ak8', 'jetSequence', 'out', PUMethod='Puppi', miniAOD=True, runOnMC=options.isMC, addSoftDrop=True , addNsub=True)  

process.options.allowUnscheduled = cms.untracked.bool(True)
##this is our example analysis module reading the results, you will have your own module
#process.heepIdExample = cms.EDAnalyzer("HEEPV70PATExample",
#                                       eles=cms.InputTag("slimmedElectrons"),
#                                       )

#process.p = cms.Path(
#    process.heepSequence*
#    process.heepIdExample) #our analysing example module, replace with your module
#


                                   
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
                              AK8recoCHSJets = cms.InputTag("slimmedJetsAK8"),
                              AK8recoPUPPIJets = cms.InputTag("selectedPatJetsAK8PFPuppi"),
#			      AK8recoPUPPIJets = cms.InputTag("AK8PFJetsPuppi"),
          		      jettinessPUPPI  = cms.untracked.string("NjettinessAK8Puppi"),

                              vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
			      edmPileupInfo = cms.InputTag('slimmedAddPileupInfo'),
                              genInfo = cms.InputTag("generator"),
                              met = cms.InputTag("slimmedMETsPuppi"),
                              trigResults = cms.InputTag("TriggerResults","","HLT"),
                              trigObjs = cms.InputTag("selectedPatTrigger"),
                              muonPathsToPass = cms.vstring("HLT_Mu50_v", "HLT_TkMu50_v"),
                              muonFiltersToPass = cms.vstring(""),
			      electronPathsToPass = cms.vstring("HLT_Ele115_CaloIdVT_GsfTrkIdT_v", "HLT_Photon175_v"),
			      electronFiltersToPass = cms.vstring(""),
                              doTrig = cms.untracked.bool(True),
                              wantHardProcessMuons = cms.untracked.bool(True),
                              doGen = cms.untracked.bool(False),
                              isMC = cms.untracked.bool(options.isMC),
			      amcatnlo = cms.untracked.bool(options.ISmcatnlo),
                              #MCL = cms.untracked.double(100),
                              #MCU = cms.untracked.double(8000),
                              outputTag = cms.untracked.string(options.gitTag)

)

process.p = cms.Path(process.muonSelectionSeq * process.heepSequence * process.analysis)
