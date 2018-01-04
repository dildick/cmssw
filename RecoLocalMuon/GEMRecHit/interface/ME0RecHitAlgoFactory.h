#ifndef RecoLocalMuon_GEMRecHit_ME0RecHitAlgoFactory_H
#define RecoLocalMuon_GEMRecHit_ME0RecHitAlgoFactory_H

#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"

typedef edmplugin::PluginFactory<ME0RecHitBaseAlgo *(const edm::ParameterSet &)> ME0RecHitAlgoFactory;

#endif
