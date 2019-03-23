#ifndef Validation_MuonME0Validation_ME0DigiMatcher_h
#define Validation_MuonME0Validation_ME0DigiMatcher_h

/**\class DigiMatcher

 Description: Matching of Digis for SimTrack in ME0

 Original Author:  "Sven Dildick"
*/

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "Validation/MuonHits/interface/ME0SimHitMatcher.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "DataFormats/GEMDigi/interface/ME0DigiPreRecoCollection.h"

#include <vector>
#include <map>
#include <set>

typedef std::vector<ME0DigiPreReco> ME0DigiPreRecoContainer;

class ME0DigiMatcher
{
public:

  // constructor
  ME0DigiMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC);

  // destructor
  ~ME0DigiMatcher() {}

  // initialize the event
  void init(const edm::Event& e, const edm::EventSetup& eventSetup);

  // do the matching
  void match(const SimTrack& t, const SimVertex& v);

  // partition ME0 detIds with digis
  std::set<unsigned int> detIds() const;

  // chamber detIds with digis
  std::set<unsigned int> chamberIds() const;

  // superchamber detIds with digis
  std::set<unsigned int> superChamberIds() const;

  // ME0 digis from a particular partition, chamber or superchamber
  const ME0DigiPreRecoContainer& digisInDetId(unsigned int) const;
  const ME0DigiPreRecoContainer& digisInChamber(unsigned int) const;
  const ME0DigiPreRecoContainer& digisInSuperChamber(unsigned int) const;

  // #layers with digis from this simtrack
  int nLayersWithDigisInSuperChamber(unsigned int) const;

  std::set<int> stripNumbersInDetId(unsigned int) const;

  // what unique partitions numbers with digis from this simtrack?
  std::set<int> partitionNumbers() const;

private:

  edm::EDGetTokenT<ME0DigiPreRecoCollection> me0DigiToken_;
  edm::Handle<ME0DigiPreRecoCollection> me0DigisH_;

  std::shared_ptr<ME0SimHitMatcher> muonSimHitMatcher_;

  edm::ESHandle<ME0Geometry> me0_geom_;
  const ME0Geometry* me0Geometry_;

  void matchPreRecoDigisToSimTrack(const ME0DigiPreRecoCollection& digis);

  bool verboseDigi_;
  int minBX_, maxBX_;
  int matchDeltaStrip_;

  std::map<unsigned int, ME0DigiPreRecoContainer> detid_to_digis_;
  std::map<unsigned int, ME0DigiPreRecoContainer> chamber_to_digis_;
  std::map<unsigned int, ME0DigiPreRecoContainer> superchamber_to_digis_;

  ME0DigiPreRecoContainer no_me0_digis_;
};

#endif
