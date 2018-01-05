import FWCore.ParameterSet.Config as cms

me0PseudoRecHits = cms.EDProducer("ME0PseudoRecHitProducer",
    recAlgoConfig = cms.PSet(),
    recAlgo = cms.string('ME0PseudoRecHitStandardAlgo'),
    me0DigiLabel = cms.InputTag("simMuonME0PseudoReDigis"),
)
