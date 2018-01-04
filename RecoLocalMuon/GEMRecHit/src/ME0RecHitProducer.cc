#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHitCollection.h"
#include "DataFormats/GEMDigi/interface/ME0DigiCollection.h"

#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitBaseAlgo.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitAlgoFactory.h"
#include "RecoLocalMuon/GEMRecHit/interface/ME0RecHitProducer.h"

#include <string>

using namespace edm;
using namespace std;


ME0RecHitProducer::ME0RecHitProducer(const ParameterSet& config)
{
  // Set verbose output
  produces<ME0RecHitCollection>();

  theME0DigiToken = consumes<ME0DigiCollection>(config.getParameter<edm::InputTag>("gemDigiLabel"));

  // Get the concrete reconstruction algo from the factory
  string theAlgoName = config.getParameter<string>("recAlgo");
  theAlgo = ME0RecHitAlgoFactory::get()->create(theAlgoName,
						config.getParameter<ParameterSet>("recAlgoConfig"));
}


ME0RecHitProducer::~ME0RecHitProducer()
{
  delete theAlgo;
}


void ME0RecHitProducer::beginRun(const edm::Run& r, const edm::EventSetup& setup)
{
}


void ME0RecHitProducer::produce(Event& event, const EventSetup& setup)
{
  // Get the ME0 Geometry
  ESHandle<ME0Geometry> gemGeom;
  setup.get<MuonGeometryRecord>().get(gemGeom);

  // Get the digis from the event
  Handle<ME0DigiCollection> digis;
  event.getByToken(theME0DigiToken,digis);

  // Pass the EventSetup to the algo
  theAlgo->setES(setup);

  // Create the pointer to the collection which will store the rechits
  auto recHitCollection = std::make_unique<ME0RecHitCollection>();

  // Iterate through all digi collections ordered by LayerId
  ME0DigiCollection::DigiRangeIterator gemdgIt;
  for (gemdgIt = digis->begin(); gemdgIt != digis->end();
       ++gemdgIt){

    // The layerId
    const ME0DetId& gemId = (*gemdgIt).first;

    // Get the GeomDet from the setup
    const ME0EtaPartition* roll = gemGeom->etaPartition(gemId);

    // Get the iterators over the digis associated with this LayerId
    const ME0DigiCollection::Range& range = (*gemdgIt).second;

    // Getting the roll mask, that includes dead strips, for the given ME0Det
    ME0EtaPartitionMask mask;

    // Call the reconstruction algorithm
    OwnVector<ME0RecHit> recHits = theAlgo->reconstruct(*roll, gemId, range, mask);

    if(!recHits.empty())
      recHitCollection->put(gemId, recHits.begin(), recHits.end());
  }

  event.put(std::move(recHitCollection));

}

