import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

from Validation.MuonHits.muonHitMatcherPSet import muonHitMatcherPSet
from Validation.MuonCSCDigis.muonCSCDigiPSet import muonCSCDigiPSet
from Validation.MuonDTDigis.muonDTDigisPSet import muonDTDigisPSet
from Validation.MuonRPCDigis.muonRPCDigiPSet import muonRPCDigiPSet
from Validation.MuonGEMDigis.muonGEMDigiPSet import muonGEMDigiPSet
from Validation.L1T.l1MuMatcherPSet import l1MuMatcherPSet

L1Validator = DQMEDAnalyzer(
    'L1Validator',
    muonHitMatcherPSet,
    muonCSCDigiPSet,
    muonDTDigisPSet,
    muonRPCDigiPSet,
    muonGEMDigiPSet,
    l1MuMatcherPSet,
    dirName=cms.string("L1T/L1TriggerVsGen"),
    #  fileName=cms.string("L1Validation.root") #output file name
    GenSource=cms.InputTag("genParticles"),
    SimTrackSource=cms.InputTag("g4SimHits"),
    SimVertexSource=cms.InputTag("g4SimHits"),
    srcToken = cms.InputTag("generator"),
    L1MuonBXSource=cms.InputTag("gmtStage2Digis", "Muon"),
    L1EGammaBXSource=cms.InputTag("caloStage2Digis", "EGamma"),
    L1TauBXSource=cms.InputTag("caloStage2Digis", "Tau"),
    L1JetBXSource=cms.InputTag("caloStage2Digis", "Jet"),
    L1ExtraMuonSource=cms.InputTag("l1extraParticles"),
    L1GenJetSource=cms.InputTag("ak4GenJets","")
)
