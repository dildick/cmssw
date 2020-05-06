from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'LLP_Data'
config.General.workArea = 'crab'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'CSCTPEmulator_cfg.py'

config.Data.inputDataset = '/ZeroBias/Run2018D-v1/RAW'
config.Data.inputDBS = 'global'
config.Data.splitting = 'Automatic' #'LumiBased'
#config.Data.unitsPerJob = 40
config.Data.outLFNDirBase = '/store/user/nimenend/All_LCT/'
config.Data.publication = True
config.Data.outputDatasetTag = 'Run2018D_ALL_LCT'
config.Data.ignoreLocality = True
config.Data.runRange = '323841-341022'

config.Site.whitelist = ["T2_US_Caltech"]
config.Site.storageSite = 'T3_US_FNALLPC'
