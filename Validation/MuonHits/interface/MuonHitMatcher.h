#ifndef Validation_MuonHits_MuonHitMatcher_h
#define Validation_MuonHits_MuonHitMatcher_h

/**\class MuonHitMatcher

   Description: Matching of muon SimHit to SimTrack

   Author: Sven Dildick (TAMU), Tao Huang (TAMU)
*/

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"
#include "Validation/MuonHits/interface/MuonHitHelper.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"

#include <vector>
#include <map>
#include <set>

class MuonHitMatcher
{
 public:

  // constructor
  MuonHitMatcher(const edm::ParameterSet& iPS, edm::ConsumesCollector && iC);

  // destructor
  ~MuonHitMatcher() {}

  // initialize the event
  void init(const edm::Event& e, const edm::EventSetup& eventSetup);

  // do the matching
  void match(const SimTrack& t, const SimVertex& v);

  // access to all the Muon SimHits (use MuonSubdetId::SubSystem)
  const edm::PSimHitContainer& simHits(int) const;

  // partitions' detIds with SimHits
  std::set<unsigned int> detIds(int type = 0) const;

  // chamber detIds with SimHits
  std::set<unsigned int> chamberIds(int type = 0) const;

  // simhits from a particular partition, chamber
  const edm::PSimHitContainer& hitsInDetId(unsigned int) const;
  const edm::PSimHitContainer& hitsInChamber(unsigned int) const;

  // calculate Global average position for a provided collection of simhits
  GlobalPoint simHitsMeanPosition(const edm::PSimHitContainer& sim_hits) const;

  // calculate Global average momentum for a provided collection of simhits in CSC
  GlobalVector simHitsMeanMomentum(const edm::PSimHitContainer& sim_hits) const;

  // calculate the average position at the second station
  GlobalPoint simHitsMeanPositionStation(int n) const;

protected:

  std::vector<unsigned int>
  getIdsOfSimTrackShower(unsigned  trk_id,
                         const edm::SimTrackContainer& simTracks,
                         const edm::SimVertexContainer& simVertices);

  bool verboseSimTrack_;
  bool simMuOnly_;
  bool discardEleHits_;
  bool verbose_;
  bool hasGeometry_;

  const TrackingGeometry* geometry_;

  edm::EDGetTokenT<edm::SimVertexContainer> simVertexInput_;
  edm::EDGetTokenT<edm::SimTrackContainer> simTrackInput_;
  edm::EDGetTokenT<edm::PSimHitContainer> simHitInput_;

  edm::Handle<edm::SimTrackContainer> simTracksH_;
  edm::Handle<edm::SimVertexContainer> simVerticesH_;
  edm::Handle<edm::PSimHitContainer> simHitsH_;

  edm::SimTrackContainer simTracks_;
  edm::SimVertexContainer simVertices_;
  // input collection
  edm::PSimHitContainer simHits_;

  std::vector<unsigned> track_ids_;
  std::map<unsigned int, unsigned int> trkid_to_index_;

  edm::PSimHitContainer no_hits_;

  // selected hits
  edm::PSimHitContainer hits_;
  std::map<unsigned int, edm::PSimHitContainer > detid_to_hits_;
  std::map<unsigned int, edm::PSimHitContainer > chamber_to_hits_;

  edm::ParameterSet simHitPSet_;
};

#endif
