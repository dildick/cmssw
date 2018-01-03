#ifndef RecoLocalMuon_GEMRecHit_ME0RecHitAlgoFactory_H
#define RecoLocalMuon_GEMRecHit_ME0RecHitAlgoFactory_H

/** \class ME0RecHitAlgoFactory
 *  Factory of seal plugins for 1D RecHit reconstruction algorithms.
 *  The plugins are concrete implementations of ME0RecHitBaseAlgo base class.
 *
 *  \author G. Cerminara - INFN Torino
 */
#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"

typedef edmplugin::PluginFactory<ME0RecHitBaseAlgo *(const edm::ParameterSet &)> ME0RecHitAlgoFactory;
#endif




