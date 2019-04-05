#ifndef Validation_MuonRPCDigis_RPCDigiMatcher_h
#define Validation_MuonRPCDigis_RPCDigiMatcher_h

/**\class DigiMatcher

 Description: Matching of Digis for SimTrack in RPC

 Author: Vadim Khotilovich, Sven Dildick (TAMU), Tao Huang (TAMU)
*/

#include "Validation/MuonHits/interface/RPCSimHitMatcher.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"

#include <vector>
#include <map>
#include <set>
#include <unordered_set>

typedef std::vector<RPCDigi> RPCDigiContainer;

class RPCDigiMatcher
{
public:

  // constructor
  RPCDigiMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC);

  // destructor
  ~RPCDigiMatcher() {}

  // initialize the event
  void init(const edm::Event& e, const edm::EventSetup& eventSetup);

  // do the matching
  void match(const SimTrack& t, const SimVertex& v);

  // partition RPC detIds with digis
  std::set<unsigned int> detIdsDigi(int rpc_type = MuonHitHelper::RPC_ALL) const;

  // chamber detIds with digis
  std::set<unsigned int> chamberIdsDigi(int rpc_type = MuonHitHelper::RPC_ALL) const;

  // RPC digis from a particular partition, chamber or superchamber
  const RPCDigiContainer& digisInDetId(unsigned int) const;
  const RPCDigiContainer& digisInChamber(unsigned int) const;

  /// How many strips in RPC did this simtrack get in total?
  int nDigis() const;

  std::set<int> stripNumbersInDetId(unsigned int) const;

  // what unique partitions numbers with digis from this simtrack?
  std::set<int> partitionNumbers() const;

  GlobalPoint getGlobalPointDigi(unsigned int rawId, const RPCDigi& d) const;

private:

  void matchDigisToSimTrack(const RPCDigiCollection&);

  edm::EDGetTokenT<RPCDigiCollection> rpcDigiToken_;

  edm::Handle<RPCDigiCollection> rpcDigisH_;

  std::unique_ptr<RPCSimHitMatcher> muonSimHitMatcher_;

  edm::ESHandle<RPCGeometry> rpc_geom_;
  const RPCGeometry* rpcGeometry_;

  template <class T>
  std::set<unsigned int> selectDetIds(const T&, int) const;

  int minRPCDigi_, maxRPCDigi_;
  int matchDeltaStrip_;
  bool verboseDigi_;

  std::map<unsigned int, RPCDigiContainer> detid_to_digis_;
  std::map<unsigned int, RPCDigiContainer> chamber_to_digis_;

  RPCDigiContainer no_rpc_digis_;
};

template <class T>
std::set<unsigned int> RPCDigiMatcher::selectDetIds(const T &digis, int rpc_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: digis)
  {
    const auto& id = p.first;
    if (rpc_type > 0)
    {
      RPCDetId detId(id);
      if (MuonHitHelper::toRPCType(detId.region(),detId.station(),detId.ring()) != rpc_type) continue;
    }
    result.insert(p.first);
  }
  return result;
}

#endif
