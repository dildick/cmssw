#include "RecoLocalMuon/GEMRecHit/interface/ME0Clusterizer.h"

ME0Clusterizer::ME0Clusterizer()
{
}

ME0Clusterizer::~ME0Clusterizer()
{
}

RecHitClusterContainer
ME0Clusterizer::doAction(const ME0DigiCollection::Range& digiRange)
{
  RecHitClusterContainer cls;
  for (auto digi = digiRange.first; digi != digiRange.second; digi++) {
    RecHitCluster cl(digi->strip(),digi->strip(),digi->bx());
    cls.insert(cl);
  }
  RecHitClusterContainer clsNew =this->doActualAction(cls);
  return clsNew;
}

RecHitClusterContainer
ME0Clusterizer::doActualAction(const RecHitClusterContainer& initialclusters) const
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


