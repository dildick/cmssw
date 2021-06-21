import FWCore.ParameterSet.Config as cms

muonGEMDigiPSet = cms.PSet(
    gemSimLink = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonGEMDigis","GEM"),
        simMuOnly = cms.bool(True),
        discardEleHits = cms.bool(True),
        run = cms.bool(True),
    ),
    gemStripDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonGEMDigis"),
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
        matchDeltaStrip = cms.int32(1),
        matchToSimLink = cms.bool(True),
        run = cms.bool(True),
    ),
    gemPadDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonGEMPadDigis"),
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
        run = cms.bool(True),
     ),
    gemPadCluster = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonGEMPadDigiClusters"),
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
        run = cms.bool(True),
     ),
    gemCoPadDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simCscTriggerPrimitiveDigis"),
        minBX = cms.int32(0),
        maxBX = cms.int32(0),
        run = cms.bool(True),
    ),
)
