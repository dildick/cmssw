#ifndef RecoLocalMuon_GEMRecHit_ME0PseudoRecHitBaseAlgo_H
#define RecoLocalMuon_GEMRecHit_ME0PseudoRecHitBaseAlgo_H

/** \class ME0PseudoRecHitBaseAlgo
 *  Abstract algorithmic class to compute Rec Hit
 *  form a ME0 digi
 *
 *  \author M. Maggi -- INFN Bari
 */

#include "DataFormats/GEMDigi/interface/ME0DigiPreRecoCollection.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHitCollection.h"

class ME0DetId;

namespace edm {
  class ParameterSet;
  class EventSetup;
}


class ME0PseudoRecHitBaseAlgo
{
 public:

  /// Constructor
  ME0PseudoRecHitBaseAlgo(const edm::ParameterSet& config);

 /// Destructor
  virtual ~ME0PseudoRecHitBaseAlgo();

  /// Pass the Event Setup to the algo at each event
  virtual void setES(const edm::EventSetup& setup) = 0;

 /// Build all hits in the range associated to the me0Id, at the 1st step.
 virtual edm::OwnVector<ME0RecHit> reconstruct(const ME0DetId& me0Id,
                                               const ME0DigiPreRecoCollection::Range& digiRange);

 /// standard local recHit computation
  virtual bool compute(const ME0DigiPreReco& digi,
                             LocalPoint& Point,
                             LocalError& error) const = 0;
};

#endif

