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
   GEMClusterContainer doAction(const GEMDetId&, GEMClusterContainer&, const GEMEtaPartitionMask& );
   int get(const GEMEtaPartitionMask&, int );
};

#endif
