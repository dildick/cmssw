#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

ME0RecHitBaseAlgo::ME0RecHitBaseAlgo(const edm::ParameterSet& config) {
}

ME0RecHitBaseAlgo::~ME0RecHitBaseAlgo(){}


// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<ME0RecHit> ME0RecHitBaseAlgo::reconstruct(const ME0EtaPartition& roll,
                                                         const ME0DetId& me0Id,
                                                         const ME0DigiCollection::Range& digiRange,
                                                         const ME0EtaPartitionMask& mask)
{
  edm::OwnVector<ME0RecHit> result;

  ME0Clusterizer clizer;
  RecHitClusterContainer tcls = clizer.doAction(digiRange);
  ME0MaskReClusterizer mrclizer;
  RecHitClusterContainer cls = mrclizer.doAction(me0Id,tcls,mask);

  for (const auto& cl : cls) {

    LocalError tmpErr;
    LocalPoint point;

    // Call the compute method
    if (!compute(roll, cl, point, tmpErr)) continue;

    // Build a new pair of 1D rechit
    int firstClustStrip = cl.firstStrip();
    int clusterSize = cl.clusterSize();

    result.push_back(ME0RecHit(me0Id,cl.bx(),firstClustStrip,clusterSize,point,tmpErr));
  }
  return result;
}
