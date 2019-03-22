import FWCore.ParameterSet.Config as cms

dtRecHit = cms.PSet(
    verbose = cms.int32(0),
    inputTag = cms.InputTag("dt1DRecHits"),
    minBX = cms.int32(-1),
    maxBX = cms.int32(1),
    )
dtRecSegment2D = cms.PSet(
    verbose = cms.int32(0),
    inputTag = cms.InputTag("dt2DSegments"),
    minBX = cms.int32(-1),
    maxBX = cms.int32(1),
    )
dtRecSegment4D = cms.PSet(
    verbose = cms.int32(0),
    inputTag = cms.InputTag("dt4DSegments"),
    minBX = cms.int32(-1),
    maxBX = cms.int32(1),
    )
