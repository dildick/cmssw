#ifndef RecoLocalMuon_GEMClusterizer_h
#define RecoLocalMuon_GEMClusterizer_h

/** \class GEMClusterizer
 *  \author M. Maggi -- INFN Bari
 */

#include "RecoLocalMuon/GEMRecHit/interface/GEMCluster.h"
#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"

class GEMClusterizer
{
 public:
  GEMClusterizer();
  ~GEMClusterizer();
  GEMClusterContainer doAction(const GEMDigiCollection::Range& digiRange);

 private:
  GEMClusterContainer doActualAction(GEMClusterContainer& initialclusters);
};

#endif
