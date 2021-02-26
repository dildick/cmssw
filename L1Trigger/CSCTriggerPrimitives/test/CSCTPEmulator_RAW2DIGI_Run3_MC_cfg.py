
# Configuration file to unpack CSC digis, run Trigger Primitives emulator,
# and compare LCTs in the data with LCTs found by the emulator.
# Slava Valuev; October, 2006.

import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process("CSCTPEmulator", eras.Run3)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring(
         '/store/mc/Run3Winter20DRPremixMiniAOD/HTo2LongLivedTo4b_MH-1000_MFF-450_CTau-100000mm_TuneCP5_14TeV_pythia8/GEN-SIM-RAW/110X_mcRun3_2021_realistic_v6-v2/10000/132B0128-FF59-DB4A-A3AD-AF4D8B4D21D2.root'
     )
)

# es_source of ideal geometry
# ===========================
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '')

# CSC raw --> digi unpacker
# =========================
process.muonCSCDigis.InputObjects = "rawDataCollector"
process.muonGEMDigis.useDBEMap = False

# CSC Trigger Primitives configuration
# ====================================

# CSC Trigger Primitives emulator
# ===============================
process.load("L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi")
process.simCscTriggerPrimitiveDigis = process.cscTriggerPrimitiveDigis.clone()

# CSC Trigger Primitives reader
# =============================
process.load("L1Trigger.CSCTriggerPrimitives.CSCTriggerPrimitivesReader_cfi")
process.lctreader.debug = True
#process.lctreader.CSCComparatorDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi")
#process.lctreader.CSCWireDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi")
process.simMuonGEMPadDigis.InputCollection = "muonGEMDigis"

# Output
# ======
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("lcts.root"),
    outputCommands = cms.untracked.vstring("keep *",
                                           "drop *_DaqSource_*_*",
                                           "drop CSCDetIdCSCStripDigiMuonDigiCollection_muonCSCDigis_MuonCSCStripDigi_CSCTPEmulator",
                                           "drop CSCDetIdCSCRPCDigiMuonDigiCollection_muonCSCDigis_MuonCSCRPCDigi_CSCTPEmulator",
                                           "drop CSCDetIdCSCStripDigiMuonDigiCollection_muonCSCDigis_MuonCSCStripDigi_*"
                                       )
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('TPEHists.root')
)

# Scheduler path
# ==============
process.p = cms.Path(
    process.muonCSCDigis*
    process.muonGEMDigis*
    process.simMuonGEMPadDigis *
    process.simMuonGEMPadDigiClusters *
    process.simCscTriggerPrimitiveDigis*
    process.lctreader
    )

#process.pp = cms.EndPath(process.output)
