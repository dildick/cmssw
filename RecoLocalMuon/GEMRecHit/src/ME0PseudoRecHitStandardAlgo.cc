#include "RecoLocalMuon/GEMRecHit/src/ME0PseudoRecHitStandardAlgo.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/Exception.h"


ME0PseudoRecHitStandardAlgo::ME0PseudoRecHitStandardAlgo(const edm::ParameterSet& config) :
  ME0PseudoRecHitBaseAlgo(config)
{
}

ME0PseudoRecHitStandardAlgo::~ME0PseudoRecHitStandardAlgo()
{
}

void ME0PseudoRecHitStandardAlgo::setES(const edm::EventSetup& setup)
{
}

// First Step
bool ME0PseudoRecHitStandardAlgo::compute(const ME0DigiPreReco& digi,
                                          LocalPoint& Point,
                                          LocalError& error)  const
{
  Point = LocalPoint(digi.x(),digi.y(),0.);
  error = LocalError(digi.ex()*digi.ex(),digi.corr()*digi.ex()*digi.ey(),digi.ey()*digi.ey());
  return true;
}

