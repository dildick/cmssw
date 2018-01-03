#ifndef RecoLocalMuon_GEMRecHit_ME0MaskReClusterizer_h
#define RecoLocalMuon_GEMRecHit_ME0MaskReClusterizer_h

#include "RecoLocalMuon/GEMRecHit/src/ME0EtaPartitionMask.h"
#include "RecoLocalMuon/GEMRecHit/src/ME0Clusterizer.h"

class ME0MaskReClusterizer
{
 public:
   ME0MaskReClusterizer();
   ~ME0MaskReClusterizer();

   ME0ClusterContainer doAction(const ME0DetId&, ME0ClusterContainer&, const ME0EtaPartitionMask&);
   int get(const ME0EtaPartitionMask&, int);
};

#endif
