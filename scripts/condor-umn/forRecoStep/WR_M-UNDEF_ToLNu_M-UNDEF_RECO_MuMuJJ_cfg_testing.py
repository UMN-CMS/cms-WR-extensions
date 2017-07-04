# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: Step1_AODSIM --mc --eventcontent RAWAODSIM --datatier GEN-SIM-RECO --conditions 80X_mcRun2_asymptotic_2016_TrancheIV_v6 --step DIGIPREMIX_S2,DATAMIX,L1,DIGI2RAW,HLT:@frozen2016,RAW2DIGI,RECO --nThreads 4 --datamix PreMix --era Run2_2016 --filein file:/hdfs/cms/phedex/store/mc/RunIISummer15GS/ZToEE_NNPDF30_13TeV-powheg_M_50_120/GEN-SIM/MCRUN2_71_V1-v1/10000/04B3B10B-7F57-E511-B9D0-0002C92DB46C.root --fileout file:step0.root --pileup_input root://cms-xrd-global.cern.ch///store/mc/RunIISpring15PrePremix/Neutrino_E-10_gun/GEN-SIM-DIGI-RAW/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v2-v2/100000/001EB167-3781-E611-BE3C-0CC47A4D75F4.root --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('HLT',eras.Run2_2016)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.DigiDMPreMix_cff')
process.load('SimGeneral.MixingModule.digi_MixPreMix_cfi')
process.load('Configuration.StandardSequences.DataMixerPreMix_cff')
process.load('Configuration.StandardSequences.SimL1EmulatorDM_cff')
process.load('Configuration.StandardSequences.DigiToRawDM_cff')
process.load('HLTrigger.Configuration.HLT_25ns15e33_v4_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(5)
    )

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10000)

    # Input source
process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring('file:/hdfs/cms/phedex/store/mc/RunIISummer15GS/ZToEE_NNPDF30_13TeV-powheg_M_50_120/GEN-SIM/MCRUN2_71_V1-v1/10000/04B3B10B-7F57-E511-B9D0-0002C92DB46C.root')
	        )
process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound')

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('Step1_AODSIM nevts:1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RAWAODSIMoutput = cms.OutputModule("PoolOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(4),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RECO'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(15728640),
    fileName = cms.untracked.string('file:step0.root'),
    outputCommands = process.RAWAODSIMEventContent.outputCommands
)

# Additional output definition

# Other statements
process.mix.digitizers = cms.PSet(process.theDigitizersMixPreMix)
process.mixData.input.fileNames = cms.untracked.vstring([
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/1840679A-5CD0-E511-9A69-0025905C96EA.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/184700A9-5DD0-E511-BBE6-008CFAF21460.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/1870FFFB-5AD0-E511-9B36-008CFA0F50F4.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/1896718B-42D0-E511-B9D6-24BE05C45BF1.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18B5A810-5DD0-E511-A546-1CC1DE1CF44E.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18BA05A5-66D0-E511-B37B-0CC47A4D760A.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18C115C3-5ED0-E511-90C4-0025904C67BA.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18CE01AC-68D0-E511-9284-D4AE52651C5F.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18D76D19-5ED0-E511-9027-0025905C96EA.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18E7E4E9-65D0-E511-ABCE-047D7BD6DEAE.root']
)
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_TrancheIV_v6', '')

# Path and EndPath definitions
process.digitisation_step = cms.Path(process.pdigi)
process.datamixing_step = cms.Path(process.pdatamix)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.raw2digi_step = cms.Path(process.RawToDigi)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWAODSIMoutput_step = cms.EndPath(process.RAWAODSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.digitisation_step,process.datamixing_step,process.L1simulation_step,process.digi2raw_step)
process.schedule.extend(process.HLTSchedule)
process.schedule.extend([process.raw2digi_step,process.reconstruction_step,process.endjob_step,process.RAWAODSIMoutput_step])

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(4)
process.options.numberOfStreams=cms.untracked.uint32(0)

# customisation of the process.

# Automatic addition of the customisation function from HLTrigger.Configuration.customizeHLTforMC
from HLTrigger.Configuration.customizeHLTforMC import customizeHLTforFullSim 

#call to customisation function customizeHLTforFullSim imported from HLTrigger.Configuration.customizeHLTforMC
process = customizeHLTforFullSim(process)

# End of customisation functions

