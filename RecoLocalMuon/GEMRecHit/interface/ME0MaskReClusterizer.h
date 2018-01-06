#ifndef RecoLocalMuon_GEMRecHit_ME0MaskReClusterizer_h
#define RecoLocalMuon_GEMRecHit_ME0MaskReClusterizer_h

#include "RecoLocalMuon/GEMRecHit/interface/ME0EtaPartitionMask.h"
#include "RecoLocalMuon/GEMRecHit/interface/Clusterizer.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"

class ME0MaskReClusterizer
{
 public:
   ME0MaskReClusterizer();
   ~ME0MaskReClusterizer();

   RecHitClusterContainer doAction(const ME0DetId&, RecHitClusterContainer&, const ME0EtaPartitionMask&);
   int get(const ME0EtaPartitionMask&, int);
};

#endif
