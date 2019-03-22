#include "Validation/DTRecHits/interface/DTRecHitMatcher.h"

using namespace std;

DTRecHitMatcher::DTRecHitMatcher(const edm::ParameterSet& pset, edm::ConsumesCollector && iC)
{
  const auto& dtRecHit1DPair = pset.getParameterSet("dtRecHit");
  maxBXDTRecHit1DPair_ = dtRecHit1DPair.getParameter<int>("maxBX");
  minBXDTRecHit1DPair_ = dtRecHit1DPair.getParameter<int>("minBX");
  verboseDTRecHit1DPair_ = dtRecHit1DPair.getParameter<int>("verbose");

  const auto& dtRecSegment2D = pset.getParameterSet("dtRecSegment2D");
  maxBXDTRecSegment2D_ = dtRecSegment2D.getParameter<int>("maxBX");
  minBXDTRecSegment2D_ = dtRecSegment2D.getParameter<int>("minBX");
  verboseDTRecSegment2D_ = dtRecSegment2D.getParameter<int>("verbose");

  const auto& dtRecSegment4D = pset.getParameterSet("dtRecSegment4D");
  maxBXDTRecSegment4D_ = dtRecSegment4D.getParameter<int>("maxBX");
  minBXDTRecSegment4D_ = dtRecSegment4D.getParameter<int>("minBX");
  verboseDTRecSegment4D_ = dtRecSegment4D.getParameter<int>("verbose");

  // make a new digi matcher
  dtDigiMatcher_.reset(new DTDigiMatcher(pset, std::move(iC)));

  dtRecHit1DPairToken_ = iC.consumes<DTRecHitCollection>(dtRecHit1DPair.getParameter<edm::InputTag>("inputTag"));
  dtRecSegment2DToken_ = iC.consumes<DTRecSegment2DCollection>(dtRecSegment2D.getParameter<edm::InputTag>("inputTag"));
  dtRecSegment4DToken_ = iC.consumes<DTRecSegment4DCollection>(dtRecSegment4D.getParameter<edm::InputTag>("inputTag"));
}

void DTRecHitMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  dtDigiMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(dtRecHit1DPairToken_, dtRecHit1DPairH_);
  iEvent.getByToken(dtRecSegment2DToken_, dtRecSegment2DH_);
  iEvent.getByToken(dtRecSegment4DToken_, dtRecSegment4DH_);

  iSetup.get<MuonGeometryRecord>().get(dt_geom_);
  if (dt_geom_.isValid()) {
    dtGeometry_ = &*dt_geom_;
  } else {
    std::cout << "+++ Info: DT geometry is unavailable. +++\n";
  }
}

/// do the matching
void DTRecHitMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match digis first
  dtDigiMatcher_->match(t,v);

  // get the rechit collection
  const DTRecHitCollection& dtRecHit1DPairs = *dtRecHit1DPairH_.product();
  const DTRecSegment2DCollection& dtRecSegment2Ds = *dtRecSegment2DH_.product();
  const DTRecSegment4DCollection& dtRecSegment4Ds = *dtRecSegment4DH_.product();

  // now match the rechits
  matchDTRecHit1DPairsToSimTrack(dtRecHit1DPairs);
  matchDTRecSegment2DsToSimTrack(dtRecSegment2Ds);
  matchDTRecSegment4DsToSimTrack(dtRecSegment4Ds);
}

void
DTRecHitMatcher::matchDTRecHit1DPairsToSimTrack(const DTRecHitCollection& rechits)
{
  /*
  if (verboseDTRecHit1DPair_) cout << "Matching simtrack to DT rechits" << endl;
  // fetch all chamberIds with simhits
  const auto& layer_ids = dtDigiMatcher_->layerIdsDT();

  for (const auto& id: layer_ids) {
    DTLayerId p_id(id);

    // print all the wires in the DTChamber
    const auto& hit_wires(dtDigiMatcher_->hitWiresInDTLayerId(id));
    if (verboseDTRecHit1DPair_) {
      cout<<"hit wires dt from simhit"<<endl;
      for (const auto& wire: hit_wires) cout << "\t"<<DTWireId(wire) << endl;
      cout<<endl;
    }

    // get the segments
    const auto& rechits_in_det = rechits.get(p_id);

    for (auto d = rechits_in_det.first; d != rechits_in_det.second; ++d) {
      if (verboseDTRecHit1DPair_) cout<<"rechit "<<p_id<<" "<<*d<<endl;

      unsigned int rightId(d->componentRecHit(DTEnums::DTCellSide::Right)->wireId());
      unsigned int leftId(d->componentRecHit(DTEnums::DTCellSide::Left)->wireId());
      set<unsigned int> rechitsIds;
      rechitsIds.insert(rightId);
      rechitsIds.insert(leftId);

      int wiresFound = 0;
      if (verboseDTRecHit1DPair_)cout<< rechitsIds.size() << " hit wires dt from rechit "<<endl;
      for (const auto& rh: rechitsIds) {
        if (verboseDTRecHit1DPair_)cout << "\t"<<rh << " " << DTWireId(rh) << endl;
        // is this "rechit" wire also a "simhit wire"?
        if (hit_wires.find(rh) != hit_wires.end()) ++wiresFound;

      }
      if (verboseDTRecHit1DPair_)cout << "Found " << wiresFound << " rechit wires out of " << hit_wires.size() << " simhit wires" << endl;
      if (wiresFound==0) continue;

      layer_to_dtRecHit1DPair_[p_id.rawId()].push_back(*d);
      superLayer_to_dtRecHit1DPair_[p_id.superlayerId().rawId()].push_back(*d);
      chamber_to_dtRecHit1DPair_[p_id.chamberId().rawId()].push_back(*d);
    }
  }
  if (verboseDTRecHit1DPair_) {
    cout << "Number of matching DTRecHit1DPairs: " << nDTRecHit1DPairs() << endl;
    for (const auto& id: chamberIdsDTRecHit1DPair()) {
      for (const auto& rh: dtRecHit1DPairInChamber(id)) {
        cout << "\t" << id << " " << rh << endl;
      }
    }
    cout<<endl;
  }
  */
}


void
DTRecHitMatcher::matchDTRecSegment2DsToSimTrack(const DTRecSegment2DCollection& dtRecSegment2Ds)
{
}


void
DTRecHitMatcher::matchDTRecSegment4DsToSimTrack(const DTRecSegment4DCollection& dtRecSegment4Ds)
{
  /*
  if (verboseDTRecSegment4D_) cout << "Matching simtrack to segments" << endl;
  // fetch all chamberIds with simhits
  const auto& chamber_ids = dtDigiMatcher_->chamberIdsDT();

  for (const auto& id: chamber_ids) {
    DTChamberId p_id(id);

    // print all the wires in the DTChamber
    const auto& hit_wires(dtDigiMatcher_->hitWiresInDTChamberId(id));
    if (verboseDTRecSegment4D_) {
      cout<<"hit wires dt from simhit"<<endl;
      for (const auto& wire: hit_wires) cout << "\t"<<DTWireId(wire) << endl;
      cout<<endl;
    }

    // get the segments
    const auto& segments_in_det = dtRecSegment4Ds.get(p_id);

    for (auto d = segments_in_det.first; d != segments_in_det.second; ++d) {
      if (verboseDTRecSegment4D_) cout<<"segment "<<p_id<<" "<<*d<<endl;

      const float time(d->hasPhi()? d->phiSegment()->t0() : d->zSegment()->t0());
      if (verboseDTRecSegment4D_) cout << "time " << time << endl;
      // check that the rechit is within BX range
      // bunch crossing is calculated from the 2D segments
      //      if (d->BunchX() < minBXGEM_ || d->BunchX() > maxBXGEM_) continue;
      // check that it matches a wire that was hit by SimHits from our track

      // access the rechits in the 4D segment
      vector<DTRecHit1D> recHits;
      if (d->hasPhi()) {
        const vector<DTRecHit1D>& phiHits = d->phiSegment()->specificRecHits();
        recHits.insert(recHits.end(), phiHits.begin(), phiHits.end());
      }
      if (d->hasZed()) {
        const vector<DTRecHit1D>& zedHits = d->zSegment()->specificRecHits();
        recHits.insert(recHits.end(), zedHits.begin(), zedHits.end());
      }

      int wiresFound = 0;
      if (verboseDTRecSegment4D_)cout<< recHits.size() << " hit wires dt from segment "<<endl;
      for (const auto& rh: recHits) {
        const auto& rhid(rh.wireId());
        if (verboseDTRecSegment4D_)cout << "\t"<<rh << " " << rhid << endl;
        // is this "rechit" wire also a "simhit wire"?
        if (hit_wires.find(rhid.rawId()) != hit_wires.end()) ++wiresFound;

      }
      if (verboseDTRecSegment4D_) cout << "Found " << wiresFound << " rechit wires out of " << hit_wires.size() << " simhit wires" << endl;
      if (wiresFound==0) continue;
      if (verboseDTRecSegment4D_) cout << "Matching DTRecSegment4D " << *d << endl;

      chamber_to_dtRecSegment4D_[ p_id.rawId() ].push_back(*d);
    }
  }
  if (verboseDTRecSegment4D_) {
    cout << "Number of matching DTRecSegment4D: " << nDTRecSegment4Ds() << endl;
    for (const auto& id: chamberIdsDTRecSegment4D()) {
      for (const auto& rh: dtRecSegment4DInChamber(id)) {
	cout << "\t" << id << " " << rh << endl;
      }
    }
    cout<<endl;
  }
  */
}


set<unsigned int>
DTRecHitMatcher::layerIdsDTRecHit1DPair() const
{
  set<unsigned int> result;
  for (const auto& p: layer_to_dtRecHit1DPair_) result.insert(p.first);
  return result;
}


set<unsigned int>
DTRecHitMatcher::superLayerIdsDTRecHit1DPair() const
{
  set<unsigned int> result;
  for (const auto& p: superLayer_to_dtRecHit1DPair_) result.insert(p.first);
  return result;
}


set<unsigned int>
DTRecHitMatcher::chamberIdsDTRecHit1DPair() const
{
  set<unsigned int> result;
  for (const auto& p: chamber_to_dtRecHit1DPair_) result.insert(p.first);
  return result;
}


set<unsigned int>
DTRecHitMatcher::superLayerIdsDTRecSegment2D() const
{
  set<unsigned int> result;
  for (const auto& p: superLayer_to_dtRecSegment2D_) result.insert(p.first);
  return result;
}


set<unsigned int>
DTRecHitMatcher::chamberIdsDTRecSegment2D() const
{
  set<unsigned int> result;
  for (const auto& p: chamber_to_dtRecSegment2D_) result.insert(p.first);
  return result;
}


set<unsigned int>
DTRecHitMatcher::chamberIdsDTRecSegment4D() const
{
  set<unsigned int> result;
  for (const auto& p: chamber_to_dtRecSegment4D_) result.insert(p.first);
  return result;
}

const DTRecHit1DPairContainer&
DTRecHitMatcher::dtRecHit1DPairInLayer(unsigned int detid) const
{
  if (layer_to_dtRecHit1DPair_.find(detid) == layer_to_dtRecHit1DPair_.end()) return no_dtRecHit1DPairs_;
  return layer_to_dtRecHit1DPair_.at(detid);
}


const DTRecHit1DPairContainer&
DTRecHitMatcher::dtRecHit1DPairInSuperLayer(unsigned int detid) const
{
  if (superLayer_to_dtRecHit1DPair_.find(detid) == superLayer_to_dtRecHit1DPair_.end()) return no_dtRecHit1DPairs_;
  return superLayer_to_dtRecHit1DPair_.at(detid);
}


const DTRecHit1DPairContainer&
DTRecHitMatcher::dtRecHit1DPairInChamber(unsigned int detid) const
{
  if (chamber_to_dtRecHit1DPair_.find(detid) == chamber_to_dtRecHit1DPair_.end()) return no_dtRecHit1DPairs_;
  return chamber_to_dtRecHit1DPair_.at(detid);
}


const DTRecSegment2DContainer&
DTRecHitMatcher::dtRecSegment2DInSuperLayer(unsigned int detid) const
{
  if (superLayer_to_dtRecSegment2D_.find(detid) == superLayer_to_dtRecSegment2D_.end()) return no_dtRecSegment2Ds_;
  return superLayer_to_dtRecSegment2D_.at(detid);
}


const DTRecSegment2DContainer&
DTRecHitMatcher::dtRecSegment2DInChamber(unsigned int detid) const
{
  if (chamber_to_dtRecSegment2D_.find(detid) == chamber_to_dtRecSegment2D_.end()) return no_dtRecSegment2Ds_;
  return chamber_to_dtRecSegment2D_.at(detid);
}


const DTRecSegment4DContainer&
DTRecHitMatcher::dtRecSegment4DInChamber(unsigned int detid) const
{
  if (chamber_to_dtRecSegment4D_.find(detid) == chamber_to_dtRecSegment4D_.end()) return no_dtRecSegment4Ds_;
  return chamber_to_dtRecSegment4D_.at(detid);
}


const DTRecHit1DPairContainer
DTRecHitMatcher::dtRecHit1DPairs() const
{
  DTRecHit1DPairContainer result;
  for (const auto& id: chamberIdsDTRecHit1DPair()){
    const auto& rechitsInChamber(dtRecHit1DPairInChamber(id));
    result.insert(result.end(), rechitsInChamber.begin(), rechitsInChamber.end());
  }
  return result;
}


const DTRecSegment2DContainer
DTRecHitMatcher::dtRecSegment2Ds() const
{
  DTRecSegment2DContainer result;
  for (const auto& id: chamberIdsDTRecSegment2D()){
    const auto& segmentsInChamber(dtRecSegment2DInChamber(id));
    result.insert(result.end(), segmentsInChamber.begin(), segmentsInChamber.end());
  }
  return result;
}


const DTRecSegment4DContainer
DTRecHitMatcher::dtRecSegment4Ds() const
{
  DTRecSegment4DContainer result;
  for (const auto& id: chamberIdsDTRecSegment4D()){
    const auto& segmentsInChamber(dtRecSegment4DInChamber(id));
    result.insert(result.end(), segmentsInChamber.begin(), segmentsInChamber.end());
  }
  return result;
}


int
DTRecHitMatcher::nDTRecSegment4DInChamber(unsigned int detid) const
{
  return dtRecSegment4DInChamber(detid).size();
}


int
DTRecHitMatcher::nDTRecHit1DPairs() const
{
  int n = 0;
  const auto& ids = chamberIdsDTRecHit1DPair();
  for (const auto& id: ids) n += dtRecHit1DPairInChamber(id).size();
  return n;
}


int
DTRecHitMatcher::nDTRecSegment2Ds() const
{
  int n = 0;
  const auto& ids = chamberIdsDTRecSegment2D();
  for (const auto& id: ids) n += dtRecSegment2DInChamber(id).size();
  return n;
}


int
DTRecHitMatcher::nDTRecSegment4Ds() const
{
  int n = 0;
  const auto& ids = chamberIdsDTRecSegment4D();
  for (const auto& id: ids) n += dtRecSegment4DInChamber(id).size();
  return n;
}

bool
DTRecHitMatcher::dtRecSegment4DInContainer(const DTRecSegment4D& thisSegment, const DTRecSegment4DContainer& c) const
{
  bool isSame = false;
  for (const auto& segment: c) if (areDTRecSegment4DSame(thisSegment,segment)) isSame = true;
  return isSame;
}


bool
DTRecHitMatcher::isDTRecSegment4DMatched(const DTRecSegment4D& thisSegment) const
{
  return dtRecSegment4DInContainer(thisSegment, dtRecSegment4Ds());
}


bool
DTRecHitMatcher::areDTRecSegment4DSame(const DTRecSegment4D& dtrh1,const DTRecSegment4D& dtrh2) const
{
  return dtrh1.localDirection()==dtrh2.localDirection() and dtrh1.localPosition()==dtrh2.localPosition();
}


