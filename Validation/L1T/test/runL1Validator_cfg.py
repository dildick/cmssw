import FWCore.ParameterSet.Config as cms

process = cms.Process("L1Val")

process.load("Validation.L1T.L1Validator_cfi")
process.load("Configuration.StandardSequences.Services_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.Geometry.GeometryExtended2023D35Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2023D35_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
#process.MessageLogger.cout.FwkReport.reportEvery = 1

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )

from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '103X_upgrade2023_realistic_v2', '')

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90003/409D7816-E20E-9142-9955-CCC52BAF2508.root',
        )
)

process.p = cms.Path(process.L1ValidatorANA)

"""

        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90003/1468130E-7991-0E46-91A4-0B085996407F.root',
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90003/8D29E3C3-69E9-F04C-A73E-5392C8522BE6.root',
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90000/8DBF6AC6-0419-7448-BB32-7EB874F5A9CD.root',
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90002/6D114E1A-5A35-584B-A372-DBA0F2F79DC4.root',
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90003/1FE0664B-4508-1047-90D1-4F7D6819B4CC.root',
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90003/1C13ED82-111C-014C-96DF-3BF6A55CBBD8.root',
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90001/D4194451-0ACA-6641-95B8-D857F690F5EA.root',
        '/store/mc/PhaseIIMTDTDRAutumn18DR/DYToLL_M-50_14TeV_TuneCP5_pythia8/FEVT/PU200_103X_upgrade2023_realistic_v2-v2/90002/2BEFF9DE-D240-2448-ACA7-FC3569E994B1.root'

"""
