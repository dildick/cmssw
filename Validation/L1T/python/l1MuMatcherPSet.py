import FWCore.ParameterSet.Config as cms

l1MuMatcherPSet = cms.PSet(

    emtfTrack = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simEmtfDigis"),
        minBX = cms.int32(0),
        maxBX = cms.int32(0),
    ),
    bmtfCand = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simBmtfDigis","EMTF"),
        minBX = cms.int32(0),
        maxBX = cms.int32(0),
    ),
    omtfCand = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simOmtfDigis","EMTF"),
        minBX = cms.int32(0),
        maxBX = cms.int32(0),
    ),
    emtfCand = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simEmtfDigis","EMTF"),
        minBX = cms.int32(0),
        maxBX = cms.int32(0),
    ),
    gmt = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simGmtStage2Digis"),
        minBX = cms.int32(0),
        maxBX = cms.int32(0),
    ),
)
