#ifndef RecoLocalMuon_GEMRecHit_ME0RecHitBaseAlgo_H
#define RecoLocalMuon_GEMRecHit_ME0RecHitBaseAlgo_H

/** \class GEMRecHitBaseAlgo
 *  Abstract algorithmic class to compute Rec Hit
 *  form a GEM digi
 *
 *  \author M. Maggi -- INFN Bari
 */

#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHitCollection.h"

#include "RecoLocalMuon/GEMRecHit/interface/ME0EtaPartitionMask.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0MaskReClusterizer.h"

class RecHitCluster;
class ME0EtaPartition;

namespace edm {
  class ParameterSet;
  class EventSetup;
}

class ME0RecHitBaseAlgo
{
 public:

  /// Constructor
  ME0RecHitBaseAlgo(const edm::ParameterSet& config);

  /// Destructor
  virtual ~ME0RecHitBaseAlgo();

  /// Pass the Event Setup to the algo at each event
  virtual void setES(const edm::EventSetup& setup) = 0;

  /// Build all hits in the range associated to the gemId, at the 1st step.
  virtual edm::OwnVector<ME0RecHit> reconstruct(const ME0EtaPartition& roll,
                                                const ME0DetId& gemId,
                                                const ME0DigiCollection::Range& digiRange,
                                                const ME0EtaPartitionMask& mask);

  /// standard local recHit computation
  virtual bool compute(const ME0EtaPartition& roll,
                       const RecHitCluster& cl,
                       LocalPoint& Point,
                       LocalError& error) const = 0;

  /// local recHit computation accounting for track direction and
  /// absolute position
  virtual bool compute(const ME0EtaPartition& roll,
                       const RecHitCluster& cl,
                       const float& angle,
                       const GlobalPoint& globPos,
                       LocalPoint& Point,
                       LocalError& error) const = 0;
};

#endif
