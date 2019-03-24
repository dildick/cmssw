#ifndef Validation_MuonRPCRecHits_RPCRecHitMatcher_h
#define Validation_MuonRPCRecHits_RPCRecHitMatcher_h

/**\class RPCRecHitMatcher

 Description: Matching of RecHits for SimTrack in RPC

*/


#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "Validation/MuonRPCDigis/interface/RPCDigiMatcher.h"

#include <vector>
#include <map>
#include <set>

typedef std::vector<RPCRecHit> RPCRecHitContainer;

class RPCGeometry;
class RPCRecHitMatcher
{
public:

  // constructor
  RPCRecHitMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC);

  // destructor
  ~RPCRecHitMatcher() {}

  // initialize the event
  void init(const edm::Event& e, const edm::EventSetup& eventSetup);

  // do the matching
  void match(const SimTrack& t, const SimVertex& v);

  // partition RPC detIds with rechits
  std::set<unsigned int> detIds() const;

  // chamber detIds with rechits
  std::set<unsigned int> chamberIds() const;

  // RPC recHits from a particular partition, chamber or superchamber
  const RPCRecHitContainer& recHitsInDetId(unsigned int) const;
  const RPCRecHitContainer& recHitsInChamber(unsigned int) const;

  const RPCRecHitContainer recHits() const;

  /// How many recHits in RPC did this simtrack get in total?
  int nRPCRecHits() const;

  std::set<int> stripNumbersInDetId(unsigned int) const;

  // what unique partitions numbers with recHits from this simtrack?
  std::set<int> partitionNumbers() const;

  // verbose value
  bool verbose() const { return verbose_; }

  // global position of the rechit (based on the first strip hit)
  GlobalPoint recHitPosition(const RPCRecHit& rechit) const;

  // mean position of a rechit collection (all based on the first strip hit)
  GlobalPoint recHitMeanPosition(const RPCRecHitContainer& rechits) const;

  std::shared_ptr<RPCDigiMatcher> rpcDigiMatcher() const { return rpcDigiMatcher_; }

  bool recHitInContainer(const RPCRecHit& rh, const RPCRecHitContainer& c) const;

  bool isRPCRecHitMatched(const RPCRecHit& thisRh) const;

  bool areRPCRecHitSame(const RPCRecHit& l, const RPCRecHit& r) const;

private:

  void matchRecHitsToSimTrack(const RPCRecHitCollection& recHits);

  edm::EDGetTokenT<RPCRecHitCollection> recHitToken_;
  edm::Handle<RPCRecHitCollection> recHitH_;

  edm::ESHandle<RPCGeometry> rpc_geom_;
  const RPCGeometry* rpcGeometry_;

  int minBX_, maxBX_;
  bool verbose_;

  std::map<unsigned int, RPCRecHitContainer> detid_to_recHits_;
  std::map<unsigned int, RPCRecHitContainer> chamber_to_recHits_;

  const RPCRecHitContainer no_recHits_;

  std::shared_ptr<RPCDigiMatcher> rpcDigiMatcher_;
};

#endif
