#ifndef RecoLocalMuon_GEMRecHit_Clusterizer_h
#define RecoLocalMuon_GEMRecHit_Clusterizer_h

#include "RecoLocalMuon/GEMRecHit/interface/RecHitCluster.h"

template <class T>
class Clusterizer
{
 public:
  Clusterizer() {};
  ~Clusterizer() {};

  void clusterize(const T& digiRange, RecHitClusterContainer& clusters) const;
};

template <class T>
void Clusterizer<T>::clusterize(const T& digiRange, RecHitClusterContainer& finalClusters) const
{
  // first step: make clusters from single digis
  RecHitClusterContainer initialClusters;
  for (auto digi = digiRange.first; digi != digiRange.second; digi++) {
    initialClusters.emplace(digi->strip(),digi->strip(),digi->bx());
  }

  // second step: merge adjancent clusters
  RecHitCluster prev;

  unsigned int j = 0;
  for(auto i = initialClusters.begin(); i != initialClusters.end(); i++) {
    RecHitCluster cl = *i;

    if(i==initialClusters.begin()) {
      prev = cl;
      j++;
      if(j == initialClusters.size()) {
        finalClusters.insert(prev);
      }
      else if(j < initialClusters.size()) {
        continue;
      }
    }

    if(prev.isAdjacent(cl)) {
      prev.merge(cl);
      j++;
      if(j == initialClusters.size()) {
        finalClusters.insert(prev);
      }
    }
    else {
      j++;
      if(j < initialClusters.size()) {
        finalClusters.insert(prev);
        prev = cl;
      }
      if(j == initialClusters.size()) {
        finalClusters.insert(prev);
        finalClusters.insert(cl);
      }
    }
  }
}


#endif
