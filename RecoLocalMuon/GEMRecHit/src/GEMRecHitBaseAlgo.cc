/*
 *  See header file for a description of this class.
 *
 *  \author M. Maggi -- INFN Bari
 */

#include "RecoLocalMuon/GEMRecHit/interface/GEMRecHitBaseAlgo.h"
#include "RecoLocalMuon/GEMRecHit/interface/Clusterizer.h"
#include "RecoLocalMuon/GEMRecHit/interface/GEMMaskReClusterizer.h"

#include "Geometry/GEMGeometry/interface/GEMEtaPartition.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


GEMRecHitBaseAlgo::GEMRecHitBaseAlgo(const edm::ParameterSet& config) {
}

GEMRecHitBaseAlgo::~GEMRecHitBaseAlgo(){}


// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<GEMRecHit> GEMRecHitBaseAlgo::reconstruct(const GEMEtaPartition& roll,
                                                         const GEMDetId& gemId,
                                                         const GEMDigiCollection::Range& digiRange,
                                                         const GEMEtaPartitionMask& mask)
{
  edm::OwnVector<GEMRecHit> result;

  Clusterizer<GEMDigiCollection::Range> clizer;
  RecHitClusterContainer tcls;
  clizer.clusterize(digiRange, tcls);

  GEMMaskReClusterizer mrclizer;
  RecHitClusterContainer cls = mrclizer.doAction(gemId,tcls,mask);

  for (const auto& cl : cls) {

    LocalError tmpErr;
    LocalPoint point;

    // Call the compute method
    if (!compute(roll, cl, point, tmpErr)) continue;

    // Build a new pair of 1D rechit
    int firstClustStrip = cl.firstStrip();
    int clusterSize = cl.clusterSize();

    result.push_back(GEMRecHit(gemId,cl.bx(),firstClustStrip,clusterSize,point,tmpErr));
  }
  return result;
}
