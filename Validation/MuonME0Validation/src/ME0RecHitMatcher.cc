#include "GEMCode/GEMValidation/interface/ME0RecHitMatcher.h"
#include "GEMCode/GEMValidation/interface/ME0DigiMatcher.h"

using namespace std;


ME0RecHitMatcher::ME0RecHitMatcher(const ME0DigiMatcher& dg,
                                   const edm::EDGetTokenT<ME0RecHitCollection>& me0RecHitInput_,
                                   const edm::EDGetTokenT<ME0SegmentCollection>& me0SegmentInput_)
  : BaseMatcher(dg.trk(), dg.vtx(), dg.conf(), dg.event(), dg.eventSetup())
  , digi_matcher_(&dg)
{
  const auto& me0RecHit = conf().getParameter<edm::ParameterSet>("me0RecHit");
  maxBXME0RecHit_ = me0RecHit.getParameter<int>("maxBX");
  minBXME0RecHit_ = me0RecHit.getParameter<int>("minBX");
  verboseME0RecHit_ = me0RecHit.getParameter<int>("verbose");
  runME0RecHit_ = me0RecHit.getParameter<bool>("run");

  const auto& me0Segment = conf().getParameter<edm::ParameterSet>("me0Segment");
  maxBXME0Segment_ = me0Segment.getParameter<int>("maxBX");
  minBXME0Segment_ = me0Segment.getParameter<int>("minBX");
  verboseME0Segment_ = me0Segment.getParameter<int>("verbose");
  runME0Segment_ = me0Segment.getParameter<bool>("run");
  minNHitsSegment_ = me0Segment.getParameter<int>("minNHits");

  if (hasME0Geometry_) {
    edm::Handle<ME0RecHitCollection> me0_rechits;
    if (gemvalidation::getByToken(me0RecHitInput_, me0_rechits, event()) and runME0RecHit_) matchME0RecHitsToSimTrack(*me0_rechits.product());
    else if (verboseME0RecHit_) std::cout <<"Not running matchME0RecHitsToSimTrack "<< std::endl;

    edm::Handle<ME0SegmentCollection> me0_segments;
    if (gemvalidation::getByToken(me0SegmentInput_, me0_segments, event()) and runME0Segment_){
      matchME0SegmentsToSimTrack(*me0_segments.product());
      matchME0SegmentsToSimTrackBydR(*me0_segments.product());
    }else if (verboseME0Segment_) std::cout <<"Not running matchME0SegmentsToSimTrack "<< std::endl;
  }
}


void
ME0RecHitMatcher::matchME0RecHitsToSimTrack(const ME0RecHitCollection& rechits)
{
  if (verboseME0RecHit_) cout << "Matching simtrack to ME0 rechits" << endl;
  // fetch all detIds with digis
  const auto& layer_ids = digi_matcher_->detIds();
  if (verboseME0RecHit_) cout << "Number of matched me0 layer_ids " << layer_ids.size() << endl;

  for (const auto& id: layer_ids) {
    ME0DetId p_id(id);

    // get the rechit
    const auto& rechits_in_det = rechits.get(p_id);
    for (auto rr = rechits_in_det.first; rr != rechits_in_det.second; ++rr) {

      // check that the rechit is within BX range
      //if (rr->tof() < minBXME0RecHit_ || rr->tof() > maxBXME0RecHit_) continue;

      if (verboseME0RecHit_) cout<<"rechit "<<p_id<<" "<<*rr << endl;;

      // match the rechit to the digis if the TOF, x and y are the same
      bool match = false;
      ME0DigiPreRecoContainer digis = digi_matcher_->digisInDetId(id);
      for (const auto& digi: digis){
	if (std::fabs(digi.tof() - rr->tof()) > 1) continue;
        if (std::fabs(digi.x() - rr->localPosition().x())<1 and
            std::fabs(digi.y() - rr->localPosition().y())<1 ) {
          match = true;
        }
      }

      // this rechit was matched to a matching digi
      if (match) {
        if (verboseME0RecHit_) cout << "\t...was matched!" << endl;
	layer_to_me0RecHit_[id].push_back(*rr);
	ME0DetId ch_id(p_id.region(), p_id.layer(), p_id.chamber(), 0);
	if (verboseME0RecHit_) cout <<"layerid "<< p_id <<" chamberid "<< ch_id <<" superId "<< p_id.chamberId() << endl;
        chamber_to_me0RecHit_[ch_id.rawId()].push_back(*rr);
        superChamber_to_me0RecHit_[p_id.chamberId().rawId()].push_back(*rr);
      }
    }
  }
}


void
ME0RecHitMatcher::matchME0SegmentsToSimTrack(const ME0SegmentCollection& me0Segments)
{
  if (verboseME0Segment_) cout << "Matching simtrack to segments" << endl;
  if (verboseME0Segment_)  dumpAllME0Segments(me0Segments);
  // fetch all chamberIds with digis
  const auto& chamber_ids = digi_matcher_->superChamberIds();
  if (verboseME0Segment_) cout << "Number of matched me0 segments " << chamber_ids.size() << endl;
  for (const auto& id: chamber_ids) {
    ME0DetId p_id(id);

    // print all ME0RecHit in the ME0SuperChamber
    const auto& me0_rechits(me0RecHitsInSuperChamber(id));
    if (verboseME0Segment_) {
      cout<<"hit me0 rechits" <<endl;
      for (const auto& rh: me0_rechits) cout << "\t"<< rh.me0Id() <<" "<< rh << endl;
      cout<<endl;
    }

    // get the segments
    bool FoundME0Segment = false;
    const auto& segments_in_det = me0Segments.get(p_id);
    for (auto d = segments_in_det.first; d != segments_in_det.second; ++d) {
      if (verboseME0Segment_) cout<<"segment "<<p_id<<" "<<*d  <<endl;

      //access the rechits
      const auto& recHits(d->recHits());

      int rechitsFound = 0;
      if (verboseME0Segment_) cout << "\t has " << recHits.size() << " me0 rechits"<<endl;
      for (const auto& rh: recHits) {
        const ME0RecHit* me0rh(dynamic_cast<const ME0RecHit*>(rh));
        if (verboseME0Segment_) cout << "Candidate rechit " << *me0rh << endl;
       	if (isME0RecHitMatched(*me0rh)) {
          if (verboseME0Segment_) cout << "\t...was matched earlier to SimTrack!" << endl;
       	  ++rechitsFound;
        }
      }
      if (rechitsFound<minNHitsSegment_) continue;
      FoundME0Segment  = true;
      if (verboseME0Segment_) {
        cout << "Found " << rechitsFound << " rechits out of " << me0RecHitsInSuperChamber(id).size() << endl;
        cout << "\t...was matched!" << endl;
      }
      superChamber_to_me0Segment_[ p_id.rawId() ].push_back(*d);
    }
    if ((not(FoundME0Segment) and nLayersWithRecHitsInSuperChamber(id)>= minNHitsSegment_)){
	  cout <<"Failed to find Segment "<< endl;
	  cout<<"Matched nlayer  "<< nLayersWithRecHitsInSuperChamber(id) <<" hit me0 rechits" <<endl;
	  for (const auto& rh: me0_rechits) cout << "\t"<< rh.me0Id() <<" "<< rh << endl;
	  cout<<endl;
    }
  }

  for (const auto& p : superChamber_to_me0Segment_)
    superChamber_to_bestME0Segment_[ p.first] = findbestME0Segment(p.second);
}

void
ME0RecHitMatcher::matchME0SegmentsToSimTrackBydR(const ME0SegmentCollection& segments)
{
  if (verboseME0Segment_) cout <<"\t matchME0SegmentsToSimTrackBydR "<< endl;
  if (verboseME0Segment_) dumpAllME0Segments( segments );
  const int endcap((trk().momentum().eta() > 0.) ? 1 : -1);
  GlobalPoint gp_propagated(propagateToZ(AVERAGE_ME0_Z*endcap));
  for(auto iC = segments.id_begin(); iC != segments.id_end(); ++iC){
    const auto& ch_segs = segments.get(*iC);
    for(auto iS = ch_segs.first; iS != ch_segs.second; ++iS){
      const GlobalPoint& gpME0(globalPoint(*iS));
      if (verboseME0Segment_)
        cout <<"ME0Detid "<< iS->me0DetId()<<" segment "<< *iS << std::endl;
      if (verboseME0Segment_)
        cout <<"gp_propagated eta "<< gp_propagated.eta() <<" phi "<< gp_propagated.phi() <<" gpME0 eta "<< gpME0.eta()<<" phi "<< gpME0.phi() << endl;
      float dPhi = gpME0.phi()- gp_propagated.phi();
      float dEta = gpME0.eta() - gp_propagated.eta();
      float dR = std::sqrt(dPhi*dPhi + dEta*dEta);
      if (dR < 0.4){
        if (verboseME0Segment_) cout << "\t matched "<< endl;
        superChamber_to_me0Segment_bydR_[ iS->me0DetId().rawId() ].push_back(*iS);
      }
    }
  }

  for (const auto& p : superChamber_to_me0Segment_bydR_)
    superChamber_to_bestME0Segment_bydR_[ p.first] = findbestME0Segment_bydR(p.second, gp_propagated);

}

void ME0RecHitMatcher::dumpAllME0Segments(const ME0SegmentCollection& segments) const
{
    cout <<"dumpt all ME0 Segments" << endl;
    for(auto iC = segments.id_begin(); iC != segments.id_end(); ++iC){
      const auto& ch_segs = segments.get(*iC);
      for(auto iS = ch_segs.first; iS != ch_segs.second; ++iS){
        const GlobalPoint& gpME0(globalPoint(*iS));
        cout <<"ME0Detid "<< iS->me0DetId()<<" segment "<< *iS <<" gp eta "<< gpME0.eta() <<" phi "<< gpME0.phi() << std::endl;
        const auto& recHits(iS->recHits());
        cout << "\t has " << recHits.size() << " me0 rechits"<<endl;
        for (const auto& rh: recHits) {
          const ME0RecHit* me0rh(dynamic_cast<const ME0RecHit*>(rh));
          cout <<"detid "<< me0rh->me0Id()<<" rechit "<< *me0rh << endl;
        }
      }
    }
}

std::set<unsigned int>
ME0RecHitMatcher::chamberIdsME0RecHit() const
{
  std::set<unsigned int> result;
  for (const auto& p: chamber_to_me0RecHit_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
ME0RecHitMatcher::superChamberIdsME0RecHit() const
{
  std::set<unsigned int> result;
  for (const auto& p: superChamber_to_me0RecHit_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
ME0RecHitMatcher::superChamberIdsME0Segment() const
{
  std::set<unsigned int> result;
  for (const auto& p: superChamber_to_me0Segment_) result.insert(p.first);
  return result;
}

std::set<unsigned int>
ME0RecHitMatcher::superChamberIdsME0Segment_bydR() const
{
  std::set<unsigned int> result;
  for (const auto& p: superChamber_to_me0Segment_bydR_) result.insert(p.first);
  return result;
}


const ME0RecHitMatcher::ME0RecHitContainer&
ME0RecHitMatcher::me0RecHitsInChamber(unsigned int detid) const
{
  if (chamber_to_me0RecHit_.find(detid) == chamber_to_me0RecHit_.end()) return no_me0RecHits_;
  return chamber_to_me0RecHit_.at(detid);
}


const ME0RecHitMatcher::ME0RecHitContainer&
ME0RecHitMatcher::me0RecHitsInSuperChamber(unsigned int detid) const
{
  if (superChamber_to_me0RecHit_.find(detid) == superChamber_to_me0RecHit_.end()) return no_me0RecHits_;
  return superChamber_to_me0RecHit_.at(detid);
}


const ME0RecHitMatcher::ME0SegmentContainer&
ME0RecHitMatcher::me0SegmentsInSuperChamber(unsigned int detid) const
{
  if (superChamber_to_me0Segment_.find(detid) == superChamber_to_me0Segment_.end()) return no_me0Segments_;
  return superChamber_to_me0Segment_.at(detid);
}

const ME0RecHitMatcher::ME0SegmentContainer&
ME0RecHitMatcher::me0SegmentsInSuperChamber_bydR(unsigned int detid) const
{
  if (superChamber_to_me0Segment_bydR_.find(detid) == superChamber_to_me0Segment_bydR_.end()) return no_me0Segments_;
  return superChamber_to_me0Segment_bydR_.at(detid);
}


int
ME0RecHitMatcher::nME0RecHitsInChamber(unsigned int detid) const
{
  return me0RecHitsInChamber(detid).size();
}


int
ME0RecHitMatcher::nME0RecHitsInSuperChamber(unsigned int detid) const
{
  return me0RecHitsInSuperChamber(detid).size();
}


int
ME0RecHitMatcher::nME0SegmentsInSuperChamber(unsigned int detid) const
{
  return me0SegmentsInSuperChamber(detid).size();
}

int
ME0RecHitMatcher::nME0SegmentsInSuperChamber_bydR(unsigned int detid) const
{
  return me0SegmentsInSuperChamber_bydR(detid).size();
}


const ME0RecHitMatcher::ME0RecHitContainer
ME0RecHitMatcher::me0RecHits() const
{
  ME0RecHitMatcher::ME0RecHitContainer result;
  for (const auto& id: superChamberIdsME0RecHit()){
    const auto& segmentsInSuperChamber(me0RecHitsInSuperChamber(id));
    result.insert(result.end(), segmentsInSuperChamber.begin(), segmentsInSuperChamber.end());
  }
  return result;
}


const ME0RecHitMatcher::ME0SegmentContainer
ME0RecHitMatcher::me0Segments() const
{
  ME0RecHitMatcher::ME0SegmentContainer result;
  for (const auto& id: superChamberIdsME0Segment()){
    const auto& segmentsInSuperChamber(me0SegmentsInSuperChamber(id));
    result.insert(result.end(), segmentsInSuperChamber.begin(), segmentsInSuperChamber.end());
  }
  return result;
}

const ME0RecHitMatcher::ME0SegmentContainer
ME0RecHitMatcher::me0Segments_bydR() const
{
  ME0RecHitMatcher::ME0SegmentContainer result;
  for (const auto& id: superChamberIdsME0Segment_bydR()){
    const auto& segmentsInSuperChamber(me0SegmentsInSuperChamber_bydR(id));
    result.insert(result.end(), segmentsInSuperChamber.begin(), segmentsInSuperChamber.end());
  }
  return result;
}

bool
ME0RecHitMatcher::me0RecHitInContainer(const ME0RecHit& rh, const ME0RecHitContainer& c) const
{
  bool isSame = false;
  for (const auto& rechit: c) if (areME0RecHitsSame(rh,rechit)) isSame = true;
  return isSame;
}


bool
ME0RecHitMatcher::me0SegmentInContainer(const ME0Segment& sg, const ME0SegmentContainer& c) const
{
  bool isSame = false;
  for (const auto& segment: c) if (areME0SegmentsSame(sg,segment)) isSame = true;
  return isSame;
}


bool
ME0RecHitMatcher::isME0RecHitMatched(const ME0RecHit& thisSg) const
{
  return me0RecHitInContainer(thisSg, me0RecHits());
}


bool
ME0RecHitMatcher::isME0SegmentMatched(const ME0Segment& thisSg) const
{
  return me0SegmentInContainer(thisSg, me0Segments());
}


int
ME0RecHitMatcher::nME0RecHits() const
{
  int n = 0;
  const auto& ids = superChamberIdsME0RecHit();
  for (const auto& id: ids) n += me0RecHitsInSuperChamber(id).size();
  return n;
}


int
ME0RecHitMatcher::nME0Segments() const
{
  int n = 0;
  const auto& ids = superChamberIdsME0Segment();
  for (const auto& id: ids) n += me0SegmentsInSuperChamber(id).size();
  return n;
}


bool
ME0RecHitMatcher::areME0RecHitsSame(const ME0RecHit& l,const ME0RecHit& r) const
{
  return ( l.localPosition() == r.localPosition() and
           l.localPositionError().xx() == r.localPositionError().xx() and
           l.localPositionError().xy() == r.localPositionError().xy() and
           l.localPositionError().yy() == r.localPositionError().yy() and
           l.tof() == r.tof() and
           l.me0Id() == r.me0Id() );
}


bool
ME0RecHitMatcher::areME0SegmentsSame(const ME0Segment& l,const ME0Segment& r) const
{
  return (l.localPosition() == r.localPosition() and l.localDirection() == r.localDirection());
}

int
ME0RecHitMatcher::nLayersWithRecHitsInSuperChamber(unsigned int detid) const
{
  set<int> layers;
  ME0DetId sch_id(detid);
  for (int iLayer=1; iLayer<=6; iLayer++){
    ME0DetId ch_id(sch_id.region(), iLayer, sch_id.chamber(), 0);
    const auto& rechits = me0RecHitsInChamber(ch_id.rawId());
    if (rechits.size()>0){
      layers.insert(iLayer);
    }
  }
  return layers.size();
}

ME0Segment
ME0RecHitMatcher::bestME0Segment(unsigned int id)
{
  if (superChamber_to_bestME0Segment_.find(id) == superChamber_to_bestME0Segment_.end()) return ME0Segment();
  else return superChamber_to_bestME0Segment_[id];
}

ME0Segment
ME0RecHitMatcher::bestME0Segment_bydR(unsigned int id) const
{
  if (superChamber_to_bestME0Segment_bydR_.find(id) == superChamber_to_bestME0Segment_bydR_.end()) return no_me0segment_;
  else  {
      ME0Segment segment(superChamber_to_bestME0Segment_bydR_.at(id));
      return segment;
  }
}

ME0Segment
ME0RecHitMatcher::findbestME0Segment(ME0SegmentContainer allSegs) const
{
  ME0Segment bestSegment;
  double chi2overNdf = 99;

  for (const auto& seg: allSegs){
    double newChi2overNdf(seg.chi2()/seg.degreesOfFreedom());
    if (newChi2overNdf < chi2overNdf) {
      chi2overNdf = newChi2overNdf;
      bestSegment = seg;
    }
  }
  return bestSegment;
}

ME0Segment
ME0RecHitMatcher::findbestME0Segment_bydR(ME0SegmentContainer allSegs, GlobalPoint gp_propagated) const
{
  ME0Segment bestSegment;
  float mindR = 0.4;

  for (const auto& seg: allSegs){
    const GlobalPoint& gpME0(globalPoint(seg));
    cout <<"gp_propagated eta "<< gp_propagated.eta() <<" phi "<< gp_propagated.phi() <<" gpME0 eta "<< gpME0.eta()<<" phi "<< gpME0.phi() << endl;
    float dPhi = gpME0.phi()- gp_propagated.phi();
    float dEta = gpME0.eta() - gp_propagated.eta();
    float dR = std::sqrt(dPhi*dPhi + dEta*dEta);
    if (dR < mindR){
	bestSegment = seg;
	mindR = dR;
    }
  }
  if (mindR>=.4)cout <<"failed to bestSegment "<< endl;
  return bestSegment;
}

GlobalPoint
ME0RecHitMatcher::globalPoint(const ME0Segment& c) const
{
  return getME0Geometry()->idToDet(c.me0DetId())->surface().toGlobal(c.localPosition());
}

float
ME0RecHitMatcher::me0DeltaPhi(ME0Segment Seg) const
{
  const auto& chamber = getME0Geometry()->chamber(Seg.me0DetId());
  float dPhi = chamber->computeDeltaPhi(Seg.localPosition(), Seg.localDirection());
  //std::cout <<"ME0detId "<< Seg.me0DetId()<<" Segment "<< Seg <<" dPhi here "<< dPhi << std::endl;
  return dPhi;

}
