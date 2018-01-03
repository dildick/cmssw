#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"
#include "RecoLocalMuon/GEMRecHit/src/ME0Cluster.h"
#include "RecoLocalMuon/GEMRecHit/src/ME0Clusterizer.h"
#include "RecoLocalMuon/GEMRecHit/src/ME0MaskReClusterizer.h"

#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


ME0RecHitBaseAlgo::ME0RecHitBaseAlgo(const edm::ParameterSet& config) {
}

ME0RecHitBaseAlgo::~ME0RecHitBaseAlgo(){}


// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<ME0RecHit> ME0RecHitBaseAlgo::reconstruct(const ME0EtaPartition& roll,
                                                         const ME0DetId& gemId,
                                                         const ME0DigiCollection::Range& digiRange,
                                                         const EtaPartitionMask& mask) {
  edm::OwnVector<ME0RecHit> result;

  ME0Clusterizer clizer;
  ME0ClusterContainer tcls = clizer.doAction(digiRange);
  ME0MaskReClusterizer mrclizer;
  ME0ClusterContainer cls = mrclizer.doAction(gemId,tcls,mask);


  for (ME0ClusterContainer::const_iterator cl = cls.begin();
       cl != cls.end(); cl++){

    LocalError tmpErr;
    LocalPoint point;
    // Call the compute method
    bool OK = this->compute(roll, *cl, point, tmpErr);
    if (!OK) continue;

    // Build a new pair of 1D rechit
    int firstClustStrip= cl->firstStrip();
    int clusterSize=cl->clusterSize();
    ME0RecHit*  recHit = new ME0RecHit(gemId,cl->bx(),firstClustStrip,clusterSize,point,tmpErr);


    result.push_back(recHit);
  }
  return result;
}
