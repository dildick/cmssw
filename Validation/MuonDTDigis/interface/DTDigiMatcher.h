#ifndef Validation_MuonDTDigis_DTDigiMatcher_h
#define Validation_MuonDTDigis_DTDigiMatcher_h

/**\class DigiMatcher

 Description: Matching of Digis for SimTrack in DT

 Original Author:  Sven Dildick
*/

#include "Validation/MuonHits/interface/MuonSimHitMatcher.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "DataFormats/DTDigi/interface/DTDigiCollection.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"

#include <vector>
#include <map>
#include <set>

class DTDigiMatcher
{
public:

  typedef std::vector<DTDigi> DTDigiContainer;
  typedef std::vector<L1MuDTChambThDigi> L1MuDTChambThDigiContainer;
  typedef std::vector<L1MuDTChambPhDigi> L1MuDTChambPhDigiContainer;

  // constructor
  DTDigiMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC);

  // destructor
  ~DTDigiMatcher() {}

  // initialize the event
  void init(const edm::Event& e, const edm::EventSetup& eventSetup);

  // do the matching
  void match(const SimTrack& t, const SimVertex& v);

  // partition GEM detIds with digis
  std::set<unsigned int> detIds(int dt_type = MuonHitHelper::DT_ALL) const;
  // chamber detIds with digis
  std::set<unsigned int> layerIds(int dt_type = MuonHitHelper::DT_ALL) const;
  // superchamber detIds with digis
  std::set<unsigned int> superLayerIds(int dt_type = MuonHitHelper::DT_ALL) const;
  // chamber detIds with digis
  std::set<unsigned int> chamberIds(int dt_type = MuonHitHelper::DT_ALL) const;

  //DT digis from a particular partition, chamber or superchamber
  const DTDigiContainer& digisInDetId(unsigned int) const;
  const DTDigiContainer& digisInLayer(unsigned int) const;
  const DTDigiContainer& digisInSuperLayer(unsigned int) const;
  const DTDigiContainer& digisInChamber(unsigned int) const;

  // #layers with digis from this simtrack
  int nLayersWithDigisInSuperLayer(unsigned int) const;
  // #layers with digis from this simtrack
  int nSuperLayersWithDigisInChamber(unsigned int) const;

  // wire numbers from this simtrack in a detId
  std::set<int> wireNumbersInDetId(unsigned int) const;

  std::shared_ptr<MuonSimHitMatcher> muonSimHitMatcher() const {return muonSimHitMatcher_;}

private:

  void matchWiresToSimTrack(const DTDigiCollection&);
  void matchThDigisToSimTrack(const L1MuDTChambThContainer&);
  void matchPhDigisToSimTrack(const L1MuDTChambPhContainer&);

  edm::EDGetTokenT<DTDigiCollection> dtWireToken_;
  edm::EDGetTokenT<L1MuDTChambThContainer> dtThDigiToken_;
  edm::EDGetTokenT<L1MuDTChambPhContainer> dtPhDigiToken_;

  edm::Handle<DTDigiCollection> dtWiresH_;
  edm::Handle<L1MuDTChambThContainer> dtThDigisH_;
  edm::Handle<L1MuDTChambPhContainer> dtPhDigisH_;

  std::shared_ptr<MuonSimHitMatcher> muonSimHitMatcher_;

  edm::ESHandle<DTGeometry> dt_geom_;
  const DTGeometry* dtGeometry_;

  std::set<unsigned int> selectDetIds(const std::map<unsigned int, DTDigiContainer>&, int) const;

  bool verboseWire_;
  int minBXDT_, maxBXDT_;
  int matchDeltaWire_;

  bool verboseThDigi_;
  int matchDeltaThDigi_;

  bool verbosePhDigi_;
  int matchDeltaPhDigi_;

  std::map<unsigned int, DTDigiContainer> detid_to_digis_;
  std::map<unsigned int, DTDigiContainer> layer_to_digis_;
  std::map<unsigned int, DTDigiContainer> superLayer_to_digis_;
  std::map<unsigned int, DTDigiContainer> chamber_to_digis_;

  std::map<unsigned int, L1MuDTChambThDigiContainer> chamber_to_th_digis_;
  std::map<unsigned int, L1MuDTChambPhDigiContainer> chamber_to_ph_digis_;

  DTDigiContainer no_dt_digis_;
  L1MuDTChambThDigiContainer no_dt_th_digis;
  L1MuDTChambThDigiContainer no_dt_ph_digis;
};

#endif
