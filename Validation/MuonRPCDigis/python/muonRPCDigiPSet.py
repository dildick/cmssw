import FWCore.ParameterSet.Config as cms

muonRPCDigiPSet = cms.PSet(
    rpcStripDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonRPCDigis"),
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
        matchDeltaStrip = cms.int32(1),
    ),
)
