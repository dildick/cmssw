import FWCore.ParameterSet.Config as cms

recoMuonPSet = cms.PSet(
    recoTrackExtra = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("L2Muons"),
    ),
    recoTrack = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("L2Muons"),
    ),
    recoChargedCandidate = cms.PSet(
        verbose = cms.int32(0),
        inputTag = cms.InputTag("L2MuonCandidates"),
    ),
)
