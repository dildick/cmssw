#ifndef RecoLocalMuon_GEMRecHit_GEMMaskReClusterizer_h
#define RecoLocalMuon_GEMRecHit_GEMMaskReClusterizer_h

/** \Class GEMMaskReClusterizer
 *  \author J.C. Sanabria -- UniAndes, Bogota
 */

#include "RecoLocalMuon/GEMRecHit/interface/GEMEtaPartitionMask.h"
#include "RecoLocalMuon/GEMRecHit/interface/GEMClusterizer.h"

class GEMMaskReClusterizer
{
 public:

   GEMMaskReClusterizer();
   ~GEMMaskReClusterizer();
   RecHitClusterContainer doAction(const GEMDetId&, RecHitClusterContainer&, const GEMEtaPartitionMask& );
   int get(const GEMEtaPartitionMask&, int );
};

#endif
