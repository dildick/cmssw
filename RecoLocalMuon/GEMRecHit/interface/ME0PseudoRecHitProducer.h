#ifndef RecoLocalMuon_GEMRecHit_ME0PseudoRecHitProducer_h
#define RecoLocalMuon_GEMRecHit_ME0PseudoRecHitProducer_h

/** \class ME0PseudoRecHitProducer
 *  Module for ME0RecHit production.
 *
 *  \author M. Maggim -- INFN Bari
 */


#include <memory>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <bitset>
#include <map>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/GEMDigi/interface/ME0DigiPreRecoCollection.h"

#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHit.h"

#include "RecoLocalMuon/GEMRecHit/interface/ME0PseudoRecHitBaseAlgo.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0PseudoRecHitAlgoFactory.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHitCollection.h"

#include <string>

class ME0PseudoRecHitBaseAlgo;

class ME0PseudoRecHitProducer : public edm::stream::EDProducer<>
{
public:
  /// Constructor
  ME0PseudoRecHitProducer(const edm::ParameterSet& config);

  /// Destructor
  ~ME0PseudoRecHitProducer() override;

  // Method that access the EventSetup for each run
  void beginRun(const edm::Run&, const edm::EventSetup& ) override;

  /// The method which produces the rechits
  void produce(edm::Event& event, const edm::EventSetup& setup) override;

private:

  // The label to be used to retrieve ME0 digis from the event
  edm::EDGetTokenT<ME0DigiPreRecoCollection> m_token;

  // The reconstruction algorithm
  ME0PseudoRecHitBaseAlgo *theAlgo;
};

#endif

