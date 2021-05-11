import FWCore.ParameterSet.Config as cms

## baseline configuration in the class itself
from EventFilter.CSCRawToDigi.cscPackerDef_cfi import cscPackerDef
cscpacker = cscPackerDef.clone()

##
## Make changes for running in Run 2
##
# packer - simply get rid of it
from Configuration.Eras.Modifier_run2_common_cff import run2_common
run2_common.toModify( cscpacker,
                      formatVersion = 2013,
                      usePreTriggers = False,
                      packEverything = True)

## in Run-3 include GEMs, and pack again the digis according to (pre)triggers
from Configuration.Eras.Modifier_run3_GEM_cff import run3_GEM
run3_GEM.toModify( cscpacker,
                   usePreTriggers = True,
                   packEverything = False,
                   packByCFEB = True,
                   padDigiClusterTag = "simMuonGEMPadDigiClusters",
                   useGEMs = False)
