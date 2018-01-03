#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "RecoLocalMuon/GEMRecHit/interface/GEMRecHitAlgoFactory.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0PseudoRecHitAlgoFactory.h"

#include "RecoLocalMuon/GEMRecHit/src/GEMRecHitProducer.h"
DEFINE_FWK_MODULE(GEMRecHitProducer);

#include "RecoLocalMuon/GEMRecHit/src/GEMRecHitStandardAlgo.h"
DEFINE_EDM_PLUGIN(GEMRecHitAlgoFactory, GEMRecHitStandardAlgo, "GEMRecHitStandardAlgo");

#include "RecoLocalMuon/GEMRecHit/src/ME0PseudoRecHitProducer.h"
DEFINE_FWK_MODULE(ME0PseudoRecHitProducer);

#include "RecoLocalMuon/GEMRecHit/src/ME0PseudoRecHitStandardAlgo.h"
DEFINE_EDM_PLUGIN(ME0PseudoRecHitAlgoFactory, ME0PseudoRecHitStandardAlgo, "ME0PseudoRecHitStandardAlgo");

