#ifndef GEMCode_GEMValidation_RPCDigiMatcher_h
#define GEMCode_GEMValidation_RPCDigiMatcher_h

/**\class DigiMatcher

 Description: Matching of Digis for SimTrack in RPC

 Original Author:  Sven Dildick
*/

#include "GEMCode/GEMValidation/interface/DigiMatcher.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include <DataFormats/RPCDigi/interface/RPCDigiCollection.h>

#include <vector>
#include <map>
#include <set>

class SimHitMatcher;

class RPCDigiMatcher : public DigiMatcher
{
public:

  typedef std::vector<RPCDigi> RPCDigiContainer;

  RPCDigiMatcher(const SimHitMatcher& sh, const edm::EDGetTokenT<RPCDigiCollection>& rpcDigiInput_);
  
  ~RPCDigiMatcher();

  // partition RPC detIds with digis
  std::set<unsigned int> detIds(int rpc_type = RPC_ALL) const;

  // chamber detIds with digis
  std::set<unsigned int> chamberIds(int rpc_type = RPC_ALL) const;


  // RPC digis from a particular partition, chamber or superchamber
  const DigiContainer& digisInDetId(unsigned int) const;
  const DigiContainer& digisInChamber(unsigned int) const;

  //RPC digis from a particular partition, chamber or superchamber
  const RPCDigiContainer& rpcDigisInDetId(unsigned int) const;
  const RPCDigiContainer& rpcDigisInChamber(unsigned int) const;

  /// How many pads in RPC did this simtrack get in total?
  int nStrips() const;

  int extrapolateHsfromRPC(unsigned int, int) const; 

  std::set<int> stripsInDetId(unsigned int) const;

  // what unique partitions numbers with digis from this simtrack?
  std::set<int> partitionNumbers() const;

private:

  void matchDigisToSimTrack(const RPCDigiCollection&);

  std::set<unsigned int> selectDetIds(const Id2DigiContainer &, int) const;

  int minBXRPC_, maxBXRPC_;

  int matchDeltaStrip_;

  std::map<unsigned int, DigiContainer> detid_to_digis_;
  std::map<unsigned int, DigiContainer> chamber_to_digis_;

  std::map<unsigned int, RPCDigiContainer> detid_to_rpcDigis_;
  std::map<unsigned int, RPCDigiContainer> chamber_to_rpcDigis_;

  bool verboseDigi_;
  bool runRPCDigi_;

  RPCDigiContainer no_rpc_digis_;
};

#endif
