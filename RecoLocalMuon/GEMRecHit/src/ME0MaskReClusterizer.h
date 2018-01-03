#ifndef RecoLocalMuon_GEMRecHit_ME0MaskReClusterizer_h
#define RecoLocalMuon_GEMRecHit_ME0MaskReClusterizer_h

/** \Class ME0MaskReClusterizer
 *  \author J.C. Sanabria -- UniAndes, Bogota
 */

#include "RecoLocalMuon/GEMRecHit/src/ME0EtaPartitionMask.h"
#include "RecoLocalMuon/GEMRecHit/src/ME0Cluster.h"
#include "RecoLocalMuon/GEMRecHit/src/ME0Clusterizer.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"

class ME0MaskReClusterizer
{
 public :

   ME0MaskReClusterizer();
   ~ME0MaskReClusterizer();
   ME0ClusterContainer doAction(const ME0DetId& ,ME0ClusterContainer& , const EtaPartitionMask& );
   int get(const EtaPartitionMask& ,int );
};

#endif
