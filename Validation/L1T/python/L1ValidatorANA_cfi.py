import FWCore.ParameterSet.Config as cms

from Validation.MuonHits.muonSimHitMatcherPSet import muonSimHitMatcherPSet
from Validation.MuonCSCDigis.muonCSCDigiPSet import muonCSCDigiPSet
from Validation.MuonCSCDigis.muonCSCStubPSet import muonCSCStubPSet
from Validation.MuonDTDigis.muonDTDigisPSet import muonDTDigisPSet
from Validation.MuonRPCDigis.muonRPCDigiPSet import muonRPCDigiPSet
from Validation.MuonGEMDigis.muonGEMDigiPSet import muonGEMDigiPSet
from Validation.L1T.l1MuMatcherPSet import l1MuMatcherPSet

L1ValidatorANA = cms.EDAnalyzer(
    'L1ValidatorANA',
    muonSimHitMatcherPSet,
    muonCSCDigiPSet,
    muonCSCStubPSet,
    muonDTDigisPSet,
    muonRPCDigiPSet,
    muonGEMDigiPSet,
    l1MuMatcherPSet,
    SimTrackSource=cms.InputTag("g4SimHits"),
    SimVertexSource=cms.InputTag("g4SimHits"),
)
