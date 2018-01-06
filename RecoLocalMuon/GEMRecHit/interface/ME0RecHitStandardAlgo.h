#ifndef RecoLocalMuon_GEMRecHit_ME0RecHitStandardAlgo_H
#define RecoLocalMuon_GEMRecHit_ME0RecHitStandardAlgo_H

/** \class ME0RecHitStandardAlgo
 *  Concrete implementation of ME0RecHitBaseAlgo.
 *
 *  \author M. Maggi -- INFN Bari
 */

#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"

class ME0RecHitStandardAlgo : public ME0RecHitBaseAlgo
{
 public:
  /// Constructor
  ME0RecHitStandardAlgo(const edm::ParameterSet& config);

  /// Destructor
  ~ME0RecHitStandardAlgo() override;

  // Operations

  /// Pass the Event Setup to the algo at each event
  void setES(const edm::EventSetup& setup) override;

  bool compute(const ME0EtaPartition& roll,
               const RecHitCluster& cluster,
               LocalPoint& point,
               LocalError& error) const override;

  bool compute(const ME0EtaPartition& roll,
               const RecHitCluster& cluster,
               const float& angle,
               const GlobalPoint& globPos,
               LocalPoint& point,
               LocalError& error) const override;
};

#endif
