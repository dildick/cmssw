#include "Validation/MuonME0Validation/interface/ME0RecHitMatcher.h"
#include "Validation/MuonME0Validation/interface/ME0DigiMatcher.h"

using namespace std;

ME0RecHitMatcher::ME0RecHitMatcher(const edm::ParameterSet& pset, edm::ConsumesCollector && iC)
{
  const auto& me0RecHit = pset.getParameter<edm::ParameterSet>("me0RecHit");
  maxBXME0RecHit_ = me0RecHit.getParameter<int>("maxBX");
  minBXME0RecHit_ = me0RecHit.getParameter<int>("minBX");
  verboseME0RecHit_ = me0RecHit.getParameter<int>("verbose");

  const auto& me0Segment = pset.getParameter<edm::ParameterSet>("me0Segment");
  maxBXME0Segment_ = me0Segment.getParameter<int>("maxBX");
  minBXME0Segment_ = me0Segment.getParameter<int>("minBX");
  verboseME0Segment_ = me0Segment.getParameter<int>("verbose");
  minNHitsSegment_ = me0Segment.getParameter<int>("minNHits");

  // make a new digi matcher
  me0DigiMatcher_.reset(new ME0DigiMatcher(pset, std::move(iC)));

  me0RecHitToken_ = iC.consumes<ME0RecHitCollection>(me0RecHit.getParameter<edm::InputTag>("inputTag"));
  me0SegmentToken_ = iC.consumes<ME0SegmentCollection>(me0Segment.getParameter<edm::InputTag>("inputTag"));
}


void ME0RecHitMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  me0DigiMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(me0RecHitToken_, me0RecHitsH_);
  iEvent.getByToken(me0SegmentToken_, me0SegmentsH_);

  iSetup.get<MuonGeometryRecord>().get(me0_geom_);
  if (me0_geom_.isValid()) {
    me0Geometry_ = &*me0_geom_;
  } else {
    std::cout << "+++ Info: ME0 geometry is unavailable. +++\n";
  }
}


/// do the matching
void ME0RecHitMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match simhits first
  me0DigiMatcher_->match(t,v);

  // get the digi collections
  const ME0RecHitCollection& me0RecHits = *me0RecHitsH_.product();
  const ME0SegmentCollection& me0Segments = *me0SegmentsH_.product();

  // now match the rechits and segments
  matchME0RecHitsToSimTrack(me0RecHits);
  matchME0SegmentsToSimTrack(me0Segments);
}


void
ME0RecHitMatcher::matchME0RecHitsToSimTrack(const ME0RecHitCollection& rechits)
{
  if (verboseME0RecHit_) cout << "Matching simtrack to ME0 rechits" << endl;
  // fetch all detIds with digis
  const auto& layer_ids = me0DigiMatcher_->detIds();
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
      ME0DigiPreRecoContainer digis = me0DigiMatcher_->digisInDetId(id);
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
  const auto& chamber_ids = me0DigiMatcher_->superChamberIds();
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
ME0RecHitMatcher::chamberIdsRecHit() const
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

const ME0RecHitContainer&
ME0RecHitMatcher::me0RecHitsInChamber(unsigned int detid) const
{
  if (chamber_to_me0RecHit_.find(detid) == chamber_to_me0RecHit_.end()) return no_me0RecHits_;
  return chamber_to_me0RecHit_.at(detid);
}


const ME0RecHitContainer&
ME0RecHitMatcher::me0RecHitsInSuperChamber(unsigned int detid) const
{
  if (superChamber_to_me0RecHit_.find(detid) == superChamber_to_me0RecHit_.end()) return no_me0RecHits_;
  return superChamber_to_me0RecHit_.at(detid);
}


const ME0SegmentContainer&
ME0RecHitMatcher::me0SegmentsInSuperChamber(unsigned int detid) const
{
  if (superChamber_to_me0Segment_.find(detid) == superChamber_to_me0Segment_.end()) return no_me0Segments_;
  return superChamber_to_me0Segment_.at(detid);
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

const ME0RecHitContainer
ME0RecHitMatcher::me0RecHits() const
{
  ME0RecHitContainer result;
  for (const auto& id: superChamberIdsME0RecHit()){
    const auto& segmentsInSuperChamber(me0RecHitsInSuperChamber(id));
    result.insert(result.end(), segmentsInSuperChamber.begin(), segmentsInSuperChamber.end());
  }
  return result;
}


const ME0SegmentContainer
ME0RecHitMatcher::me0Segments() const
{
  ME0SegmentContainer result;
  for (const auto& id: superChamberIdsME0Segment()){
    const auto& segmentsInSuperChamber(me0SegmentsInSuperChamber(id));
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

GlobalPoint
ME0RecHitMatcher::globalPoint(const ME0Segment& c) const
{
  return me0Geometry_->idToDet(c.me0DetId())->surface().toGlobal(c.localPosition());
}

float
ME0RecHitMatcher::me0DeltaPhi(ME0Segment Seg) const
{
  const auto& chamber = me0Geometry_->chamber(Seg.me0DetId());
  return chamber->computeDeltaPhi(Seg.localPosition(), Seg.localDirection());

}
