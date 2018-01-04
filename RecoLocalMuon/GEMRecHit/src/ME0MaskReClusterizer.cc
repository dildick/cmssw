#include "RecoLocalMuon/GEMRecHit/interface/ME0MaskReClusterizer.h"

ME0MaskReClusterizer::ME0MaskReClusterizer()
{

}


ME0MaskReClusterizer::~ME0MaskReClusterizer()
{

}


RecHitClusterContainer ME0MaskReClusterizer::doAction(const ME0DetId& id,
                                                   RecHitClusterContainer& initClusters,
                                                   const ME0EtaPartitionMask& mask)
{

  RecHitClusterContainer finClusters;
  RecHitCluster prev;

  unsigned int j = 0;


  for (RecHitClusterContainer::const_iterator i = initClusters.begin(); i != initClusters.end(); i++ ) {

    RecHitCluster cl = *i;

    if ( i == initClusters.begin() ) {
      prev = cl;
      j++;
      if ( j == initClusters.size() ) {
	finClusters.insert(prev);
      }
      else if ( j < initClusters.size() ) {
	continue;
      }
    }


    if ( ((prev.firstStrip()-cl.lastStrip()) == 2 && this->get(mask,(cl.lastStrip()+1)))
	 && (cl.bx() == prev.bx()) ) {

      RecHitCluster merged(cl.firstStrip(),prev.lastStrip(),cl.bx());
      prev = merged;
      j++;
      if ( j == initClusters.size() ) {
	finClusters.insert(prev);
      }
    }

    else {

      j++;
      if ( j < initClusters.size() ) {
	finClusters.insert(prev);
	prev = cl;
      }
      if ( j == initClusters.size() ) {
	finClusters.insert(prev);
	finClusters.insert(cl);
      }
    }

  }

  return finClusters;

}



int ME0MaskReClusterizer::get(const ME0EtaPartitionMask& mask, int strip)
{

  if ( mask.test(strip-1) ) return 1;
  else return 0;

}
