/** \Class GEMMaskReClusterizer
 *  \author J.C. Sanabria -- UniAndes, Bogota
 */

#include "RecoLocalMuon/GEMRecHit/interface/GEMMaskReClusterizer.h"

GEMMaskReClusterizer::GEMMaskReClusterizer()
{

}


GEMMaskReClusterizer::~GEMMaskReClusterizer()
{

}


RecHitClusterContainer GEMMaskReClusterizer::doAction(const GEMDetId& id,
                                                    RecHitClusterContainer& initClusters,
                                                    const GEMEtaPartitionMask& mask)
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



int GEMMaskReClusterizer::get(const GEMEtaPartitionMask& mask, int strip)
{

  if ( mask.test(strip-1) ) return 1;
  else return 0;

}
