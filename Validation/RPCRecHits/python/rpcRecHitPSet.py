import FWCore.ParameterSet.Config as cms

rpcRecHit = cms.PSet(
    verbose = cms.int32(0),
    inputTag = cms.InputTag("rpcRecHits"),
    minBX = cms.int32(-1),
    maxBX = cms.int32(1),
)
