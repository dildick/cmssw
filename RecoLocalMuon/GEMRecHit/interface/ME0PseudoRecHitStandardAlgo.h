#ifndef RecoLocalMuon_GEMRecHit_ME0PseudoRecHitStandardAlgo_H
#define RecoLocalMuon_GEMRecHit_ME0PseudoRecHitStandardAlgo_H

/** \class ME0PseudoRecHitStandardAlgo
 *  Concrete implementation of ME0PseudoRecHitBaseAlgo.
 *
 *  \author M. Maggi -- INFN Bari
 */
#include "RecoLocalMuon/GEMRecHit/interface/ME0PseudoRecHitBaseAlgo.h"

class ME0PseudoRecHitStandardAlgo : public ME0PseudoRecHitBaseAlgo
{
 public:
  /// Constructor
  ME0PseudoRecHitStandardAlgo(const edm::ParameterSet& config);

  /// Destructor
  ~ME0PseudoRecHitStandardAlgo() override;

  /// Pass the Event Setup to the algo at each event
  void setES(const edm::EventSetup& setup) override;


  bool compute(const ME0DigiPreReco& digi,
               LocalPoint& point,
               LocalError& error) const override;
};
#endif


