/*
* See header file for a description of this class.
*
* \author M. Maggi -- INFN Bari
*/

#include "RecoLocalMuon/GEMRecHit/interface/ME0PseudoRecHitBaseAlgo.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "DataFormats/GEMDigi/interface/ME0DigiPreRecoCollection.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

ME0PseudoRecHitBaseAlgo::ME0PseudoRecHitBaseAlgo(const edm::ParameterSet& config) {}

ME0PseudoRecHitBaseAlgo::~ME0PseudoRecHitBaseAlgo(){}


// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<ME0RecHit> ME0PseudoRecHitBaseAlgo::reconstruct(const ME0DetId& me0Id,
                                                               const ME0DigiPreRecoCollection::Range& digiRange)
{
  edm::OwnVector<ME0RecHit> result;

  for (auto digi = digiRange.first; digi != digiRange.second;digi++) {

    LocalError tmpErr;
    LocalPoint point;
    // Call the compute method
    bool OK = this->compute(*digi, point, tmpErr);
    if (!OK) continue;
    result.push_back(ME0RecHit(me0Id,digi->tof(),point,tmpErr));
  }
  return result;
}
