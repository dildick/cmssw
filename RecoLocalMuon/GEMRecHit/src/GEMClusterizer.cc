#include "RecoLocalMuon/GEMRecHit/interface/GEMClusterizer.h"

GEMClusterizer::GEMClusterizer()
{
}

GEMClusterizer::~GEMClusterizer()
{
}

RecHitClusterContainer
GEMClusterizer::doAction(const GEMDigiCollection::Range& digiRange)
{
  RecHitClusterContainer cls;
  for (auto digi = digiRange.first; digi != digiRange.second; digi++) {
    RecHitCluster cl(digi->strip(),digi->strip(),digi->bx());
    cls.insert(cl);
  }
  RecHitClusterContainer clsNew = doActualAction(cls);
  return clsNew;
}

RecHitClusterContainer
GEMClusterizer::doActualAction(const RecHitClusterContainer& initialclusters)
{
  RecHitClusterContainer finalCluster;
  RecHitCluster prev;

  unsigned int j = 0;
  for(RecHitClusterContainer::const_iterator i=initialclusters.begin();
      i != initialclusters.end(); i++){
    RecHitCluster cl = *i;

    if(i==initialclusters.begin()){
      prev = cl;
      j++;
      if(j == initialclusters.size()){
        finalCluster.insert(prev);
      }
      else if(j < initialclusters.size()){
        continue;
      }
    }

    if(prev.isAdjacent(cl)) {
      prev.merge(cl);
      j++;
      if(j == initialclusters.size()){
        finalCluster.insert(prev);
      }
    }
    else {
      j++;
      if(j < initialclusters.size()){
        finalCluster.insert(prev);
        prev = cl;
      }
      if(j == initialclusters.size()){
        finalCluster.insert(prev);
        finalCluster.insert(cl);
      }
    }
  }

  return finalCluster;
}


