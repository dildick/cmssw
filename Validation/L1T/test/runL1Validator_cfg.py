import FWCore.ParameterSet.Config as cms

process = cms.Process("L1Val")

process.load("Validation.L1T.L1Validator_cfi")
process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
#        'file:step2_RAW2DIGI_RECO_VALIDATION_DQM.root'
    )
)

process.p = cms.Path(process.L1Validator)
