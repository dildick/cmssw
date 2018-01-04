#ifndef RecoLocalMuon_GEMRecHit_ME0RecHitProducer_h
#define RecoLocalMuon_GEMRecHit_ME0RecHitProducer_h

/** \class ME0RecHitProducer
 *  Module for ME0RecHit production.
 *
 *  \author M. Maggi -- INFN Bari
 */


#include <memory>
#include <cstdint>
#include <cstdlib>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"

#include "RecoLocalMuon/GEMRecHit/interface/ME0EtaPartitionMask.h"

namespace edm {
  class ParameterSet;
  class Event;
  class EventSetup;
}

class ME0RecHitBaseAlgo;

class ME0RecHitProducer : public edm::stream::EDProducer<> {

public:
  /// Constructor
  ME0RecHitProducer(const edm::ParameterSet& config);

  /// Destructor
  ~ME0RecHitProducer() override;

  // Method that access the EventSetup for each run
  void beginRun(const edm::Run&, const edm::EventSetup& ) override;

  /// The method which produces the rechits
  void produce(edm::Event& event, const edm::EventSetup& setup) override;

private:

  // The token to be used to retrieve ME0 digis from the event
  edm::EDGetTokenT<ME0DigiCollection> theME0DigiToken;

  // The reconstruction algorithm
  ME0RecHitBaseAlgo *theAlgo;
};

#endif

