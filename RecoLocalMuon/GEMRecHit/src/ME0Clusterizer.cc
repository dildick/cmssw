#include "RecoLocalMuon/GEMRecHit/interface/ME0Clusterizer.h"

ME0Clusterizer::ME0Clusterizer()
{
}

ME0Clusterizer::~ME0Clusterizer()
{
}

ME0ClusterContainer
ME0Clusterizer::doAction(const ME0DigiCollection::Range& digiRange)
{
  ME0ClusterContainer cls;
  for (auto digi = digiRange.first; digi != digiRange.second; digi++) {
    ME0Cluster cl(digi->strip(),digi->strip(),digi->bx());
    cls.insert(cl);
  }
  ME0ClusterContainer clsNew =this->doActualAction(cls);
  return clsNew;
}

ME0ClusterContainer
ME0Clusterizer::doActualAction(const ME0ClusterContainer& initialclusters) const
{
  ME0ClusterContainer finalCluster;
  ME0Cluster prev;

  unsigned int j = 0;
  for(ME0ClusterContainer::const_iterator i=initialclusters.begin();
      i != initialclusters.end(); i++){
    ME0Cluster cl = *i;

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


