from __future__ import print_function

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_cff import Run3

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.parseArguments()

## process def
process = cms.Process("TEST", Run3)
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration/StandardSequences/GeometryRecoDB_cff")
process.load("Configuration/StandardSequences/MagneticField_cff")
process.load("Configuration/StandardSequences/FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load("EventFilter.CSCRawToDigi.cscUnpacker_cfi")
process.load("EventFilter.CSCRawToDigi.cscPacker_cfi")
process.load("EventFilter.CSCRawToDigi.cscViewDigiDef_cfi")

process.maxEvents = cms.untracked.PSet(
      input = cms.untracked.int32(1)
)

process.source = cms.Source(
      "PoolSource",
      fileNames = cms.untracked.vstring(options.inputFiles)
)

## global tag
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '')

process.out = cms.OutputModule(
      "PoolOutputModule",
      fileName = cms.untracked.string('output.root'),
)

## pretriggers are not saved in the packing-unpacking
process.viewDigiRAW = process.cscViewDigiDef.clone(
    ClctPreTriggerDump = False,
)

process.viewDigiSIM = process.viewDigiRAW.clone(
    wireTag = "simMuonCSCDigis:MuonCSCWireDigi",
    stripTag = "simMuonCSCDigis:MuonCSCStripDigi",
    comparatorTag = "simMuonCSCDigis:MuonCSCComparatorDigi",
    alctTag = "simCscTriggerPrimitiveDigis",
    clctTag = "simCscTriggerPrimitiveDigis",
    corrclctTag = "simCscTriggerPrimitiveDigis",
    ClctPreTriggerDump = True
)

process.muonCSCDigis.InputObjects = "cscpacker:CSCRawData"

## schedule and path definition
process.p1 = cms.Path(
    process.viewDigiSIM *
    process.cscpacker *
    process.muonCSCDigis *
    process.viewDigiRAW
)
process.endjob_step = cms.EndPath(process.out * process.endOfProcess)

process.schedule = cms.Schedule(process.p1, process.endjob_step)
