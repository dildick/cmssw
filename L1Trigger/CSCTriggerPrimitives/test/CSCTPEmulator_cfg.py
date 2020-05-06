# Configuration file to unpack CSC digis, run Trigger Primitives emulator,
# and compare LCTs in the data with LCTs found by the emulator.
# Slava Valuev; October, 2006.

import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run2_2018_cff import Run2_2018
process = cms.Process("CSCTPEmulator", Run2_2018)

process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(100)
)

# Hack to add "test" directory to the python path.
import sys, os
sys.path.insert(0, os.path.join(os.environ['CMSSW_BASE'],
                                'src/L1Trigger/CSCTriggerPrimitives/test'))

process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring(
         'file:lcts.root'
     )
)

"""
'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file1_to_5.root',
'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file6_to_10.root',
'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file11_to_15.root',
'file:/afs/cern.ch/work/c/cpena/public/NikTrigger/step2_file16_to_20.root'
#         'file:/uscms/home/dildick/nobackup/work/LLPStudiesWithSergoEtAL/CMSSW_10_6_4/src/step2.root'
"""

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
        "lctreader")
)

# es_source of ideal geometry
# ===========================
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '103X_dataRun2_Prompt_v3', '')

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
process.cscTriggerPrimitiveDigis.CSCComparatorDigiProducer = "muonCSCDigis:MuonCSCComparatorDigi"
process.cscTriggerPrimitiveDigis.CSCWireDigiProducer = "muonCSCDigis:MuonCSCWireDigi"

# CSC Trigger Primitives reader
# =============================
process.load("L1Trigger.CSCTriggerPrimitives.CSCTriggerPrimitivesReader_cfi")
process.lctreader.debug = False
#process.lctreader.CSCComparatorDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi")
#process.lctreader.CSCWireDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi")

# Output
# ======
process.output = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string("lcts_emulated.root"),
    outputCommands = cms.untracked.vstring("drop *",
                                           "keep *_cscTriggerPrimitive*_*_*",
                                           "keep *_*csc*_*_*",
                                           "keep *_*CSC*_*_*",
					   "drop CSCDetIdCSCStripDigiMuonDigiCollection_muonCSCDigis_MuonCSCStripDigi_CSCTPEmulator",
					   "drop CSCDetIdCSCRPCDigiMuonDigiCollection_muonCSCDigis_MuonCSCRPCDigi_CSCTPEmulator",
                                           "drop CSCDetIdCSCStripDigiMuonDigiCollection_muonCSCDigis_MuonCSCStripDigi_*"
                                       )
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('TPEHists_emulated.root')
)

# Scheduler path
# ==============
process.p = cms.Path(
    process.muonCSCDigis*
    process.cscTriggerPrimitiveDigis
    *process.lctreader
    )

process.pp = cms.EndPath(process.output)
