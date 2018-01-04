#ifndef RecoLocalMuon_GEMRecHit_ME0Clusterizer_h
#define RecoLocalMuon_GEMRecHit_ME0Clusterizer_h

#include "RecoLocalMuon/GEMRecHit/interface/ME0Cluster.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"

class ME0Clusterizer
{
 public:
  ME0Clusterizer();
  ~ME0Clusterizer();

  ME0ClusterContainer doAction(const ME0DigiCollection::Range& digiRange);

 private:
  ME0ClusterContainer doActualAction(const ME0ClusterContainer& initialclusters) const;
};
#endif
