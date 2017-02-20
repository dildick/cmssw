import FWCore.ParameterSet.Config as cms

generator = cms.EDProducer("FlatRandomPtAndDxyGunProducer",
    PGunParameters = cms.PSet(
        PartID = cms.vint32(-13),
        MinPt  = cms.double(2.00),
        MaxPt  = cms.double(50.00),
        MinEta = cms.double(-3.0),
        MaxEta = cms.double(3.0),
        MinPhi = cms.double(-3.14159265359),
        MaxPhi = cms.double(3.14159265359),
        LzMin  = cms.double(-500.0),#in cm
        LzMax  = cms.double(500.0),#in cm
        dxyMin = cms.double(-50.0),
        dxyMax = cms.double(50.0)
    ),
    Verbosity = cms.untracked.int32(0), ## set to 1 (or greater)  for printouts
    psethack = cms.string('displaced muon'),
    AddAntiParticle = cms.bool(True),
    firstRun = cms.untracked.uint32(1)
)
