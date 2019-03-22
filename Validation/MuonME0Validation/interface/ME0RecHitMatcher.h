#ifndef GEMCode_GEMValidation_ME0RecHitMatcher_h
#define GEMCode_GEMValidation_ME0RecHitMatcher_h

/**\class DigiMatcher

 Description: Matching of rechits and segments for SimTrack in CSC

 Original Author:  Sven Dildick
*/

#include "GEMCode/GEMValidation/interface/BaseMatcher.h"
#include "GEMCode/GEMValidation/interface/ME0DigiMatcher.h"

#include "DataFormats/GEMRecHit/interface/ME0RecHitCollection.h"
#include "DataFormats/GEMRecHit/interface/ME0SegmentCollection.h"

#include <vector>
#include <map>
#include <set>

class ME0RecHitMatcher : public BaseMatcher
{
public:

  typedef std::vector<ME0RecHit> ME0RecHitContainer;
  typedef std::vector<ME0Segment> ME0SegmentContainer;

  ME0RecHitMatcher(const ME0DigiMatcher& sh,
                   const edm::EDGetTokenT<ME0RecHitCollection>& me0RecHitInput_,
                   const edm::EDGetTokenT<ME0SegmentCollection>& me0SegmentInput_);

  ~ME0RecHitMatcher() {}

  // chamber detIds with ME0RecHit
  std::set<unsigned int> chamberIdsME0RecHit() const;
  // superChamber detIds with ME0RecHit
  std::set<unsigned int> superChamberIdsME0RecHit() const;
  // superChamber detIds with ME0Segment
  std::set<unsigned int> superChamberIdsME0Segment() const;
  std::set<unsigned int> superChamberIdsME0Segment_bydR() const;

  void dumpAllME0Segments(const ME0SegmentCollection& segments) const;
  //ME0 rechits from a particular chamber or superChamber
  const ME0RecHitContainer& me0RecHitsInChamber(unsigned int) const;
  const ME0RecHitContainer& me0RecHitsInSuperChamber(unsigned int) const;
  //ME0 segments from a particular superChamber
  const ME0SegmentContainer& me0SegmentsInSuperChamber(unsigned int) const;
  const ME0SegmentContainer& me0SegmentsInSuperChamber_bydR(unsigned int) const;

  const ME0SegmentContainer me0Segments() const;
  const ME0SegmentContainer me0Segments_bydR() const;
  const ME0RecHitContainer me0RecHits() const;

  bool me0RecHitInContainer(const ME0RecHit&, const ME0RecHitContainer&) const;
  bool me0SegmentInContainer(const ME0Segment&, const ME0SegmentContainer&) const;
  bool me0SegmentInContainer_bydR(const ME0Segment&, const ME0SegmentContainer&) const;
  bool isME0RecHitMatched(const ME0RecHit&) const;
  bool isME0SegmentMatched(const ME0Segment&) const;

  int nME0RecHits() const;
  int nME0Segments() const;
  bool areME0SegmentsSame(const ME0Segment&,const ME0Segment&) const;
  bool areME0RecHitsSame(const ME0RecHit&,const ME0RecHit&) const;

  int nME0RecHitsInChamber(unsigned int) const;
  int nME0RecHitsInSuperChamber(unsigned int) const;
  int nME0SegmentsInSuperChamber(unsigned int) const;
  int nME0SegmentsInSuperChamber_bydR(unsigned int) const;
  int nLayersWithRecHitsInSuperChamber(unsigned int detid) const;

  float me0DeltaPhi(ME0Segment Seg) const;

  ME0Segment bestME0Segment(unsigned int);
  ME0Segment bestME0Segment_bydR(unsigned int) const ;
  ME0Segment findbestME0Segment(ME0SegmentContainer allSegs) const ;
  ME0Segment findbestME0Segment_bydR(ME0SegmentContainer allSegs, GlobalPoint gp) const ;

  GlobalPoint globalPoint(const ME0Segment&) const;

private:

  const ME0DigiMatcher* digi_matcher_;

  void matchME0RecHitsToSimTrack(const ME0RecHitCollection&);
  void matchME0SegmentsToSimTrack(const ME0SegmentCollection&);
  void matchME0SegmentsToSimTrackBydR(const ME0SegmentCollection&);


  int verboseME0RecHit_;
  bool runME0RecHit_;
  int maxBXME0RecHit_;
  int minBXME0RecHit_;

  int verboseME0Segment_;
  bool runME0Segment_;
  int maxBXME0Segment_;
  int minBXME0Segment_;
  int minNHitsSegment_;

  //ME0Segment bestMe0Seg_;
  std::map<unsigned int, ME0Segment> superChamber_to_bestME0Segment_;
  std::map<unsigned int, ME0Segment> superChamber_to_bestME0Segment_bydR_;

  std::map<unsigned int, ME0RecHitContainer> layer_to_me0RecHit_;
  std::map<unsigned int, ME0RecHitContainer> chamber_to_me0RecHit_;
  std::map<unsigned int, ME0RecHitContainer> superChamber_to_me0RecHit_;
  std::map<unsigned int, ME0SegmentContainer> superChamber_to_me0Segment_;
  std::map<unsigned int, ME0SegmentContainer> superChamber_to_me0Segment_bydR_;

  ME0RecHitContainer no_me0RecHits_;
  ME0SegmentContainer no_me0Segments_;
  ME0Segment no_me0segment_;
};

#endif
