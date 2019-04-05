import FWCore.ParameterSet.Config as cms

me0HitPSet = cms.PSet(
    me0StripDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonME0PseudoReDigis"),
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
        matchDeltaStrip = cms.int32(1),
        ),
    me0RecHit = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag('me0RecHits'),
        simMuOnly = cms.bool(True),
        discardEleHits = cms.bool(True),
        ),
    me0Seg = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag('me0Segments'),
        simMuOnly = cms.bool(True),
        discardEleHits = cms.bool(True),
        )
)
