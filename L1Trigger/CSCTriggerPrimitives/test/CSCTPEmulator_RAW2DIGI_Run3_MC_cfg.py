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
         '/store/user/nimenend/ppTohToSSTo4b/ppTohToSSTo4b_DIGI_L1/200513_135650/0000/step2_8.root'
     )
)

'file:/uscms/home/dildick/nobackup/work/LLPStudiesWithSergoEtAL/CMSSW_11_1_0_pre6/src/L1Trigger/CSCTriggerPrimitives/test/FFC59020-EA48-1F41-B4B8-FF34C0E09D88.root'

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
