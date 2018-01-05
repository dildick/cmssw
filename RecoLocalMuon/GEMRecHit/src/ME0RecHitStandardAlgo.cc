#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitStandardAlgo.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/Exception.h"


ME0RecHitStandardAlgo::ME0RecHitStandardAlgo(const edm::ParameterSet& config) :
  ME0RecHitBaseAlgo(config)
{
}


ME0RecHitStandardAlgo::~ME0RecHitStandardAlgo()
{
}


void ME0RecHitStandardAlgo::setES(const edm::EventSetup& setup) {
}


// First Step
bool ME0RecHitStandardAlgo::compute(const ME0EtaPartition& roll,
                                    const RecHitCluster& cluster,
                                    LocalPoint& Point,
                                    LocalError& error)  const
{
  // Get Average Strip position
  float fstrip = (roll.centreOfStrip(cluster.firstStrip())).x();
  float lstrip = (roll.centreOfStrip(cluster.lastStrip())).x();
  float centreOfCluster = (fstrip + lstrip)/2;

  Point = LocalPoint(centreOfCluster,0.,0.);
  error = roll.localError((cluster.firstStrip()+cluster.lastStrip())/2., cluster.clusterSize());
  return true;
}


bool ME0RecHitStandardAlgo::compute(const ME0EtaPartition& roll,
                                    const RecHitCluster& cl,
                                    const float& angle,
                                    const GlobalPoint& globPos,
                                    LocalPoint& Point,
                                    LocalError& error)  const
{

  // Glob Pos and angle not used so far...
  if (globPos.z()<0){ } // Fake use to avoid warnings
  if (angle<0.){ }      // Fake use to avoid warnings
  this->compute(roll,cl,Point,error);
  return true;
}

