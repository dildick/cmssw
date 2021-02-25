# Configuration file to unpack CSC digis, run Trigger Primitives emulator,
# and compare LCTs in the data with LCTs found by the emulator.
# Slava Valuev; October, 2006.

import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process("ANA", eras.Run3)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-100)
)

process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring(
         '/store/user/menendez/HTo2LongLivedTo4q_MH_125_MFF_1_CTau_10000mm_TuneCP5_14TeV_pythia/HTo2LongLivedTo4q_MH_125_MFF_1_CTau_10000mm_TuneCP5_14TeV_pythia/200710_130547/0000/step2_1.root'
     )
)

'''
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
'''

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
process.lctreader.compData = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi")
process.lctreader.wireData = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi")
process.lctreader.alctData = process.lctreader.alctEmul
process.lctreader.clctData = process.lctreader.clctEmul
process.lctreader.lctData = process.lctreader.lctEmul
process.lctreader.mpclctData = process.lctreader.mpclctEmul

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
    process.lctreader
    )

#process.pp = cms.EndPath(process.output)
