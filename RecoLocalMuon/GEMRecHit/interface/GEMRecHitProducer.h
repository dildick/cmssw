#ifndef RecoLocalMuon_GEMRecHit_GEMRecHitProducer_h
#define RecoLocalMuon_GEMRecHit_GEMRecHitProducer_h

/** \class GEMRecHitProducer
 *  Module for GEMRecHit production.
 *
 *  \author M. Maggim -- INFN Bari
 */


#include <memory>
#include <stdint.h>
#include <stdlib.h>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"

#include "CondFormats/GEMObjects/interface/GEMMaskedStrips.h"
#include "CondFormats/DataRecord/interface/GEMMaskedStripsRcd.h"
#include "CondFormats/GEMObjects/interface/GEMDeadStrips.h"
#include "CondFormats/DataRecord/interface/GEMDeadStripsRcd.h"

#include "RecoLocalMuon/GEMRecHit/interface/GEMEtaPartitionMask.h"


namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

class GEMRecHitBaseAlgo;

class GEMRecHitProducer : public edm::stream::EDProducer<> {

public:
  /// Constructor
  GEMRecHitProducer(const edm::ParameterSet& config);

  /// Destructor
  ~GEMRecHitProducer() override {};

  // Method that access the EventSetup for each run
  void beginRun(const edm::Run&, const edm::EventSetup& ) override;

  /// The method which produces the rechits
  void produce(edm::Event& event, const edm::EventSetup& setup) override;

private:

  // The token to be used to retrieve GEM digis from the event
  edm::EDGetTokenT<GEMDigiCollection> theGEMDigiToken;

  // The reconstruction algorithm
  std::unique_ptr<GEMRecHitBaseAlgo> theAlgo;

  // Object with mask-strips-vector for all the GEM Detectors
  std::unique_ptr<GEMMaskedStrips> GEMMaskedStripsObj;

  // Object with dead-strips-vector for all the GEM Detectors
  std::unique_ptr<GEMDeadStrips> GEMDeadStripsObj;

  std::string maskSource;
  std::string deadSource;

  std::vector<GEMMaskedStrips::MaskItem> MaskVec;
  std::vector<GEMDeadStrips::DeadItem> DeadVec;
};

#endif

