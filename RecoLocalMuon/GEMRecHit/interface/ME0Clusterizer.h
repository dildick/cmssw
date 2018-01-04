#ifndef RecoLocalMuon_GEMRecHit_ME0Clusterizer_h
#define RecoLocalMuon_GEMRecHit_ME0Clusterizer_h

#include "RecoLocalMuon/GEMRecHit/interface/RecHitCluster.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"

class ME0Clusterizer
{
 public:
  ME0Clusterizer();
  ~ME0Clusterizer();

  RecHitClusterContainer doAction(const ME0DigiCollection::Range& digiRange);

 private:
  RecHitClusterContainer doActualAction(const RecHitClusterContainer& initialclusters) const;
};
#endif
