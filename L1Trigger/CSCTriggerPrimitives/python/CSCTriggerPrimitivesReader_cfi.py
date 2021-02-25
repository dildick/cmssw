import FWCore.ParameterSet.Config as cms

from L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi import cscTriggerPrimitiveDigis

# Default parameters for CSCTriggerPrimitives analyzer
# =====================================================
lctreader = cms.EDAnalyzer(
    "CSCTriggerPrimitivesReader",
    # Parameters common for all boards
    commonParam = cscTriggerPrimitiveDigis.commonParam,
    # Switch on/off the verbosity and turn on/off histogram production
    debug = cms.bool(False),
    # Define which LCTs are present in the input file.
    # This will determine the workflow of the Reader.
    mcTruthIn = cms.bool(True),
    dataLctsIn = cms.bool(True),
    emulLctsIn = cms.bool(True),
    checkBadChambers = cms.bool(True),
    dataIsAnotherMC = cms.bool(True),
    printps = cms.bool(True),
    resultsFileNamesPrefix = cms.string(""),
    #data:
    compData = cms.InputTag("muonCSCDigis","MuonCSCComparatorDigi"),
    wireData = cms.InputTag("muonCSCDigis","MuonCSCWireDigi"),
    alctData = cms.InputTag("muonCSCDigis", "MuonCSCALCTDigi"),
    clctData = cms.InputTag("muonCSCDigis", "MuonCSCCLCTDigi"),
    lctData = cms.InputTag("muonCSCDigis", "MuonCSCCorrelatedLCTDigi"),
    mpclctData = cms.InputTag("emtfStage2Digis"),
    #simulation(emulator):
    genParticles      = cms.InputTag("genParticles"),
    CSCSimHitProducer = cms.InputTag("g4SimHits", "MuonCSCHits"),  # Full sim.
    compEmul = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi"),
    wireEmul = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi"),
    alctEmul = cms.InputTag("simCscTriggerPrimitiveDigis"),
    clctEmul = cms.InputTag("simCscTriggerPrimitiveDigis"),
    preclctEmul = cms.InputTag("simCscTriggerPrimitiveDigis"),
    lctEmul = cms.InputTag("simCscTriggerPrimitiveDigis"),
    mpclctEmul = cms.InputTag("simCscTriggerPrimitiveDigis","MPCSORTED"),
)
