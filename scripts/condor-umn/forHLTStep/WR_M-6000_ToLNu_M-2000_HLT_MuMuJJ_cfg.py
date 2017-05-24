# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: --mc --step DIGI,L1,DIGI2RAW,HLT:25ns10e33_v2 --filein=file:WR-2400_ToLNu-1200_ToEEJJ_NEW-GEN-SIM.root --fileout=file:WR_M-2400_ToLNu_M-1200_HLT_NEW.root --python_filename=WR_M-2400_ToLNu_M-1200_HLT_NEW.py --eventcontent=RAWSIM --datatier=GEN-SIM-RAW --conditions=80X_mcRun2_asymptotic_2016_miniAODv2_v1 --era=Run2_2016 --pileup_input=dbs:/MinBias_TuneCUETP8M1_13TeV-pythia8/RunIISummer15GS-MCRUN2_71_V1-v2/GEN-SIM --pileup=Flat_10_50_25ns -n 100 --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process('HLT',eras.Run2_2016)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mix_Flat_10_50_25ns_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('HLTrigger.Configuration.HLT_25ns10e33_v2_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    fileNames = cms.untracked.vstring('file:WR_M-2400_ToLNu_M-1200_GEN-SIM_13TeV-2016.root')
    ,
    inputCommands = cms.untracked.vstring('keep *', 
        'drop *_genParticlesForJets_*_*', 
        'drop *_kt4GenJets_*_*', 
        'drop *_kt6GenJets_*_*', 
        'drop *_iterativeCone5GenJets_*_*', 
        'drop *_ak4GenJets_*_*', 
        'drop *_ak7GenJets_*_*', 
        'drop *_ak4GenJetsNoNu_*_*', 
        'drop *_ak8GenJetsNoNu_*_*', 
        'drop *_genCandidatesForMET_*_*', 
        'drop *_genParticlesForMETAllVisible_*_*', 
        'drop *_genMetCalo_*_*', 
        'drop *_genMetCaloAndNonPrompt_*_*', 
        'drop *_genMetTrue_*_*', 
        'drop *_genMetIC5GenJs_*_*'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('--mc nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-RAW'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('file:WR_M-6000_ToLNu_M-2000_RAWSIM_13TeV-2016.root'),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
process.mix.input.fileNames = cms.untracked.vstring([
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/1840679A-5CD0-E511-9A69-0025905C96EA.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/184700A9-5DD0-E511-BBE6-008CFAF21460.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/1870FFFB-5AD0-E511-9B36-008CFA0F50F4.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/1896718B-42D0-E511-B9D6-24BE05C45BF1.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18B5A810-5DD0-E511-A546-1CC1DE1CF44E.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18BA05A5-66D0-E511-B37B-0CC47A4D760A.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18C115C3-5ED0-E511-90C4-0025904C67BA.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18CE01AC-68D0-E511-9284-D4AE52651C5F.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18D76D19-5ED0-E511-9027-0025905C96EA.root',
'file:/data/whybee0b/user/aevans/RunIISummer15GS_MinBias_TuneCUETP8M1_13TeV-pythia8_GEN-SIM_MCRUN2_71_V1_ext1-v1/18E7E4E9-65D0-E511-ABCE-047D7BD6DEAE.root'
])

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_miniAODv2_v1', '')  

# Path and EndPath definitions
process.digitisation_step = cms.Path(process.pdigi)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.digitisation_step,process.L1simulation_step,process.digi2raw_step)
process.schedule.extend(process.HLTSchedule)
process.schedule.extend([process.endjob_step,process.RAWSIMoutput_step])

# customisation of the process.

# Automatic addition of the customisation function from HLTrigger.Configuration.customizeHLTforMC
from HLTrigger.Configuration.customizeHLTforMC import customizeHLTforFullSim 

#call to customisation function customizeHLTforFullSim imported from HLTrigger.Configuration.customizeHLTforMC
process = customizeHLTforFullSim(process)

# End of customisation functions
