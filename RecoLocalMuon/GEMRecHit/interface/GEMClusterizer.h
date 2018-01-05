#ifndef RecoLocalMuon_GEMRecHit_GEMClusterizer_h
#define RecoLocalMuon_GEMRecHit_GEMClusterizer_h

/** \class GEMClusterizer
 *  \author M. Maggi -- INFN Bari
 */

#include "RecoLocalMuon/GEMRecHit/interface/RecHitCluster.h"
#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"

class GEMClusterizer
{
 public:
  GEMClusterizer();
  ~GEMClusterizer();
  RecHitClusterContainer doAction(const GEMDigiCollection::Range& digiRange);

 private:
  RecHitClusterContainer doActualAction(const RecHitClusterContainer& initialclusters);
};

#endif
