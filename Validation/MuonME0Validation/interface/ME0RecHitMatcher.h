#ifndef Validation_MuonME0Validation_ME0RecHitMatcher_h
#define Validation_MuonME0Validation_ME0RecHitMatcher_h

/**\class DigiMatcher

 Description: Matching of rechits and segments for SimTrack in CSC

 Original Author:  Sven Dildick
*/

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "Validation/MuonME0Validation/interface/ME0DigiMatcher.h"
#include "DataFormats/GEMRecHit/interface/ME0RecHitCollection.h"
#include "DataFormats/GEMRecHit/interface/ME0SegmentCollection.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"

#include <vector>
#include <map>
#include <set>

typedef std::vector<ME0RecHit> ME0RecHitContainer;
typedef std::vector<ME0Segment> ME0SegmentContainer;

class ME0RecHitMatcher
{
public:

  // constructor
  ME0RecHitMatcher(const edm::ParameterSet& iPS, edm::ConsumesCollector && iC);

  // destructor
  ~ME0RecHitMatcher() {}

  // initialize the event
  void init(const edm::Event& e, const edm::EventSetup& eventSetup);

  // do the matching
  void match(const SimTrack& t, const SimVertex& v);

  // chamber detIds with ME0RecHit
  std::set<unsigned int> chamberIdsRecHit() const;

  // superChamber detIds with ME0RecHit
  std::set<unsigned int> superChamberIdsME0RecHit() const;

  // superChamber detIds with ME0Segment
  std::set<unsigned int> superChamberIdsME0Segment() const;

  void dumpAllME0Segments(const ME0SegmentCollection& segments) const;

  //ME0 rechits from a particular chamber or superChamber
  const ME0RecHitContainer& me0RecHitsInChamber(unsigned int) const;
  const ME0RecHitContainer& me0RecHitsInSuperChamber(unsigned int) const;

  //ME0 segments from a particular superChamber
  const ME0SegmentContainer& me0SegmentsInSuperChamber(unsigned int) const;

  const ME0SegmentContainer me0Segments() const;
  const ME0RecHitContainer me0RecHits() const;

  bool me0RecHitInContainer(const ME0RecHit&, const ME0RecHitContainer&) const;
  bool me0SegmentInContainer(const ME0Segment&, const ME0SegmentContainer&) const;

  bool isME0RecHitMatched(const ME0RecHit&) const;
  bool isME0SegmentMatched(const ME0Segment&) const;

  int nME0RecHits() const;
  int nME0Segments() const;
  bool areME0SegmentsSame(const ME0Segment&,const ME0Segment&) const;
  bool areME0RecHitsSame(const ME0RecHit&,const ME0RecHit&) const;

  int nME0RecHitsInChamber(unsigned int) const;
  int nME0RecHitsInSuperChamber(unsigned int) const;
  int nME0SegmentsInSuperChamber(unsigned int) const;
  int nLayersWithRecHitsInSuperChamber(unsigned int detid) const;

  float me0DeltaPhi(ME0Segment Seg) const;

  ME0Segment bestME0Segment(unsigned int);
  ME0Segment findbestME0Segment(ME0SegmentContainer allSegs) const ;

  GlobalPoint globalPoint(const ME0Segment&) const;

private:

  edm::EDGetTokenT<ME0RecHitCollection> me0RecHitToken_;
  edm::EDGetTokenT<ME0SegmentCollection> me0SegmentToken_;

  edm::Handle<ME0RecHitCollection> me0RecHitsH_;
  edm::Handle<ME0SegmentCollection> me0SegmentsH_;

  edm::ESHandle<ME0Geometry> me0_geom_;
  const ME0Geometry* me0Geometry_;

  std::unique_ptr<ME0DigiMatcher> me0DigiMatcher_;

  void matchME0RecHitsToSimTrack(const ME0RecHitCollection&);
  void matchME0SegmentsToSimTrack(const ME0SegmentCollection&);

  int verboseME0RecHit_;
  int maxBXME0RecHit_;
  int minBXME0RecHit_;

  int verboseME0Segment_;
  int maxBXME0Segment_;
  int minBXME0Segment_;
  int minNHitsSegment_;

  //ME0Segment bestMe0Seg_;
  std::map<unsigned int, ME0Segment> superChamber_to_bestME0Segment_;
  std::map<unsigned int, ME0RecHitContainer> layer_to_me0RecHit_;
  std::map<unsigned int, ME0RecHitContainer> chamber_to_me0RecHit_;
  std::map<unsigned int, ME0RecHitContainer> superChamber_to_me0RecHit_;
  std::map<unsigned int, ME0SegmentContainer> superChamber_to_me0Segment_;

  ME0RecHitContainer no_me0RecHits_;
  ME0SegmentContainer no_me0Segments_;
  ME0Segment no_me0segment_;
};

#endif
