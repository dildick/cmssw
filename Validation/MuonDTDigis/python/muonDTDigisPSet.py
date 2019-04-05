import FWCore.ParameterSet.Config as cms

muonDTDigisPSet = cms.PSet(
    dtWireDigi = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simMuonDTDigis"),
        ## not sure which BX is the central one
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
        matchDeltaWire = cms.int32(1),
        minNHitsChamber = cms.int32(4),
    ),
    dtPhiDigiTrigger = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simDtTriggerPrimitiveDigis"),
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
    ),
    dtThetaDigiTrigger = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("simDtTriggerPrimitiveDigis"),
        minBX = cms.int32(-1),
        maxBX = cms.int32(1),
    ),
)
