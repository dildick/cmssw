#ifndef RecoLocalMuon_GEMRecHit_ME0PseudoRecHitAlgoFactory_H
#define RecoLocalMuon_GEMRecHit_ME0PseudoRecHitAlgoFactory_H

/** \class ME0PseudoRecHitAlgoFactory
 *  Factory of seal plugins for 1D RecHit reconstruction algorithms.
 *  The plugins are concrete implementations of ME0PseudoRecHitBaseAlgo base class.
 *
 *  \author M. Maggi - INFN Torino
 */
#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0PseudoRecHitBaseAlgo.h"

typedef edmplugin::PluginFactory<ME0PseudoRecHitBaseAlgo *(const edm::ParameterSet &)> ME0PseudoRecHitAlgoFactory;

#endif

