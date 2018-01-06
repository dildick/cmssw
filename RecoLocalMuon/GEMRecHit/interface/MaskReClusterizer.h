#ifndef RecoLocalMuon_GEMRecHit_MaskReClusterizer_h
#define RecoLocalMuon_GEMRecHit_MaskReClusterizer_h

/** \Class MaskReClusterizer
 *  \author J.C. Sanabria -- UniAndes, Bogota
 */

#include "RecoLocalMuon/GEMRecHit/interface/Clusterizer.h"

template <class T>
class MaskReClusterizer
{
 public:

  MaskReClusterizer() {};
  ~MaskReClusterizer() {};

   void reclusterize(const RecHitClusterContainer& initialClusters,
                     RecHitClusterContainer& finalClusters,
                     const T& mask) const;

   int get(const T& mask, int i) const;
};

template <class T>
void MaskReClusterizer<T>::reclusterize(const RecHitClusterContainer& initialClusters,
                                        RecHitClusterContainer& finalClusters,
                                        const T& mask) const
{
  RecHitCluster prev;
  unsigned int j = 0;

  for (auto i = initialClusters.begin(); i != initialClusters.end(); i++ ) {

    RecHitCluster cl = *i;

    if ( i == initialClusters.begin() ) {
      prev = cl;
      j++;
      if ( j == initialClusters.size() ) {
        finalClusters.insert(prev);
      }
      else if ( j < initialClusters.size() ) {
        continue;
      }
    }

    if ( prev.firstStrip()-cl.lastStrip() == 2 &&
         get(mask,(cl.lastStrip()+1)) &&
         cl.bx() == prev.bx() ) {

      RecHitCluster merged(cl.firstStrip(),prev.lastStrip(),cl.bx());
      prev = merged;
      j++;
      if ( j == initialClusters.size() ) {
        finalClusters.insert(prev);
      }
    }

    else {
      j++;
      if ( j < initialClusters.size() ) {
        finalClusters.insert(prev);
        prev = cl;
      }
      if ( j == initialClusters.size() ) {
        finalClusters.insert(prev);
        finalClusters.insert(cl);
      }
    }
  }
}

template <class T>
int MaskReClusterizer<T>::get(const T& mask, int strip) const
{
  if ( mask.test(strip-1) ) return 1;
  else return 0;
}

#endif
