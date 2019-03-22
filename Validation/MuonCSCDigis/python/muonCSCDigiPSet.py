import FWCore.ParameterSet.Config as cms

muonCSCDigiPSet = cms.PSet(
    #csc wire digi, central BX 7
    comparatorDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonCSCDigis", "MuonCSCComparatorDigi"),
        minBX = cms.int32(5),
        maxBX = cms.int32(11),
        matchDeltaStrip = cms.int32(2),
        minNHitsChamber = cms.int32(4),
    ),
    stripDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonCSCDigis", "MuonCSCStripDigi"),
        minBX = cms.int32(5),
        maxBX = cms.int32(11),
        matchDeltaStrip = cms.int32(2),
        minNHitsChamber = cms.int32(4),
    ),
    #csc wire digi, central BX 8
    wireDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonCSCDigis", "MuonCSCWireDigi"),
        minBX = cms.int32(5),
        maxBX = cms.int32(11),
        matchDeltaWG = cms.int32(2),
        minNHitsChamber = cms.int32(4),
    ),
)
