# Configuration file to unpack CSC digis, run Trigger Primitives emulator,
# and compare LCTs in the data with LCTs found by the emulator.
# Slava Valuev; October, 2006.

import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process("ANA", eras.Run3)

process.maxEvents = cms.untracked.PSet(
    #input = cms.untracked.int32(100)
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring(
         ##'file:/uscms/home/dildick/nobackup/work/LLPStudiesWithSergoEtAL/CMSSW_10_6_4/src/step2.root'
         'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file1_to_5.root',
         'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file6_to_10.root',
         'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file11_to_15.root',
         'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file16_to_20.root'
         #'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/CMSSW_10_6_4/src/../../Data/data_v2/HIG-RunIIFall18wmLHEGS-01282_FEVTDEBUG_LLP_WH_15.root'

         #'/store/mc/PhaseIITDRSpring19DR/Nu_E10-pythia8-gun/GEN-SIM-DIGI-RAW/PU200_106X_upgrade2023_realistic_v3-v3/70000/EDE4B502-F1D1-2446-ACED-49A370F253E1.root'
         #'file:/afs/cern.ch/work/s/sixie/public/releases/run3/CSCL1Trigger/CMSSW_10_6_4/src/TPEHists.signal.root'
  
         #'root://cmsxrootd.fnal.gov//store/user/menendez/ZeroBias/Run2018D_All_LCT/200210_213120/0000/lcts_725.root'
         #'file:lcts_emulated.root'
         #'file:TPEHists.root'
     )
)

process.MessageLogger = cms.Service("MessageLogger",
   destinations = cms.untracked.vstring("debug"),
   debug = cms.untracked.PSet(
       extension = cms.untracked.string(".txt"),
       threshold = cms.untracked.string("DEBUG"),
       # threshold = cms.untracked.string("WARNING"),
       lineLength = cms.untracked.int32(132),
       noLineBreaks = cms.untracked.bool(True)
   ),
   debugModules = cms.untracked.vstring("cscTriggerPrimitiveDigis",
                                        "lctreader","lctDigis","nearestWG", "nearestHS")
)

# es_source of ideal geometry
# ===========================
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '')

# magnetic field (do I need it?)
# ==============================
process.load('Configuration.StandardSequences.MagneticField_cff')

# CSC raw --> digi unpacker
# =========================
process.load("EventFilter.CSCRawToDigi.cscUnpacker_cfi")
process.muonCSCDigis.InputObjects = "rawDataCollector"

# CSC Trigger Primitives configuration
# ====================================

# CSC Trigger Primitives emulator
# ===============================
process.load("L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi")
#process.cscTriggerPrimitiveDigis.alctParam07.verbosity = 2
#process.cscTriggerPrimitiveDigis.clctParam07.verbosity = 2
#process.cscTriggerPrimitiveDigis.tmbParam.verbosity = 2
#process.cscTriggerPrimitiveDigis.CSCComparatorDigiProducer = "simMuonCSCDigis:MuonCSCComparatorDigi:HLT"
#process.cscTriggerPrimitiveDigis.CSCWireDigiProducer = "simMuonCSCDigis:MuonCSCWireDigi:HLT"

# CSC Trigger Primitives reader
# =============================
process.load("L1Trigger.CSCTriggerPrimitives.CSCTriggerPrimitivesReader_cfi")
process.lctreader.debug = True
#process.lctreader.CSCComparatorDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi")
#process.lctreader.CSCWireDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi")

# Output
# ======
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("lcts.root"),
    outputCommands = cms.untracked.vstring("keep *",
                                           "drop *_DaqSource_*_*",
                                           "drop *"
                                       )
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('TPEHists.root')
)

# Scheduler path
# ==============
process.p = cms.Path(#process.muonCSCDigis*
    #process.muonCSCDigis
    #process.cscTriggerPrimitiveDigis*
    process.lctreader
    )

process.pp = cms.EndPath(process.output)

