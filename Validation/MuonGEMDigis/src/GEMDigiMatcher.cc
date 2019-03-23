#include "Validation/MuonGEMDigis/interface/GEMDigiMatcher.h"

using namespace std;

GEMDigiMatcher::GEMDigiMatcher(const edm::ParameterSet& pset, edm::ConsumesCollector && iC)
{
  const auto& gemDigi = pset.getParameterSet("gemStripDigi");
  minGEMDigi_ = gemDigi.getParameter<int>("minBX");
  maxGEMDigi_ = gemDigi.getParameter<int>("maxBX");
  matchDeltaStrip_ = gemDigi.getParameter<int>("matchDeltaStrip");
  verboseDigi_ = gemDigi.getParameter<int>("verbose");

  const auto& gemPad = pset.getParameterSet("gemPadDigi");
  minGEMPad_ = gemPad.getParameter<int>("minBX");
  maxGEMPad_ = gemPad.getParameter<int>("maxBX");
  verbosePad_ = gemPad.getParameter<int>("verbose");

  const auto& gemCoPad = pset.getParameterSet("gemCoPadDigi");
  minGEMCoPad_ = gemCoPad.getParameter<int>("minBX");
  maxGEMCoPad_ = gemCoPad.getParameter<int>("maxBX");
  verboseCoPad_ = gemCoPad.getParameter<int>("verbose");

  // make a new simhits matcher
  muonHitMatcher_.reset(new MuonHitMatcher(pset, std::move(iC)));

  gemDigiToken_ = iC.consumes<GEMDigiCollection>(gemDigi.getParameter<edm::InputTag>("inputTag"));
  gemPadToken_ = iC.consumes<GEMPadDigiCollection>(gemPad.getParameter<edm::InputTag>("inputTag"));
  gemCoPadToken_ = iC.consumes<GEMCoPadDigiCollection>(gemCoPad.getParameter<edm::InputTag>("inputTag"));
}

void GEMDigiMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  muonHitMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(gemDigiToken_, gemDigisH_);
  iEvent.getByToken(gemPadToken_, gemPadsH_);
  iEvent.getByToken(gemCoPadToken_, gemCoPadsH_);

  iSetup.get<MuonGeometryRecord>().get(gem_geom_);
  if (gem_geom_.isValid()) {
    gemGeometry_ = &*gem_geom_;
  } else {
    std::cout << "+++ Info: GEM geometry is unavailable. +++\n";
  }
}

/// do the matching
void GEMDigiMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match simhits first
  muonHitMatcher_->match(t,v);

  // get the digi collections
  const GEMDigiCollection& gemDigis = *gemDigisH_.product();
  const GEMPadDigiCollection& gemPads = *gemPadsH_.product();
  const GEMCoPadDigiCollection& gemCoPads = *gemCoPadsH_.product();

  // now match the digis
  matchDigisToSimTrack(gemDigis);
  matchPadsToSimTrack(gemPads);
  matchCoPadsToSimTrack(gemCoPads);
}


void
GEMDigiMatcher::matchDigisToSimTrack(const GEMDigiCollection& digis)
{
  if (verboseDigi_) cout << "Matching simtrack to GEM digis" << endl;
  const auto& det_ids = muonHitMatcher_->detIdsGEM();
  for (const auto& id: det_ids)
  {
    GEMDetId p_id(id);
    const auto& hit_strips = muonHitMatcher_->hitStripsInDetId(id, matchDeltaStrip_);
    if (verboseDigi_)
    {
      cout<<"hit_strips_fat ";
      copy(hit_strips.begin(), hit_strips.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }

    const auto& digis_in_det = digis.get(GEMDetId(id));

    for (auto d = digis_in_det.first; d != digis_in_det.second; ++d)
    {
      if (verboseDigi_) cout<<"GEMDigi "<<p_id<<" "<<*d<<endl;
      // check that the digi is within BX range
      if (d->bx() < minGEMDigi_ || d->bx() > maxGEMDigi_) continue;
      // check that it matches a strip that was hit by SimHits from our track
      if (hit_strips.find(d->strip()) == hit_strips.end()) continue;
      if (verboseDigi_) cout<<"...was matched!"<<endl;

      detid_to_digis_[id].push_back(*d);
      chamber_to_digis_[ p_id.chamberId().rawId() ].push_back(*d);
      superchamber_to_digis_[ p_id.superChamberId().rawId() ].push_back(*d);
    }
  }
}


void
GEMDigiMatcher::matchPadsToSimTrack(const GEMPadDigiCollection& pads)
{
  const auto& det_ids = muonHitMatcher_->detIdsGEM();
  for (const auto& id: det_ids)
  {
    GEMDetId p_id(id);
    GEMDetId superch_id(p_id.region(), p_id.ring(), p_id.station(), 0, p_id.chamber(), 0);

    const auto& hit_pads = muonHitMatcher_->hitPadsInDetId(id);
    const auto& pads_in_det = pads.get(p_id);

    if (verbosePad_)
    {
      cout<<"checkpads "<<hit_pads.size()<<" "<<std::distance(pads_in_det.first, pads_in_det.second)<<" hit_pads: ";
      copy(hit_pads.begin(), hit_pads.end(), ostream_iterator<int>(cout," "));
      cout<<endl;
    }

    for (auto pad = pads_in_det.first; pad != pads_in_det.second; ++pad)
    {
      if (verbosePad_) cout<<"chp "<<*pad<<endl;
      // check that the pad BX is within the range
      if (pad->bx() < minGEMPad_ || pad->bx() > maxGEMPad_) continue;
      if (verbosePad_) cout<<"chp1"<<endl;
      // check that it matches a pad that was hit by SimHits from our track
      if (hit_pads.find(pad->pad()) == hit_pads.end()) continue;
      if (verbosePad_) cout<<"chp2"<<endl;

      detid_to_pads_[id].push_back(*pad);
      chamber_to_pads_[ p_id.chamberId().rawId() ].push_back(*pad);
      superchamber_to_pads_[ superch_id() ].push_back(*pad);
    }
  }
}


void
GEMDigiMatcher::matchCoPadsToSimTrack(const GEMCoPadDigiCollection& co_pads)
{
  const auto& det_ids = muonHitMatcher_->detIdsGEMCoincidences();
  for (const auto& id: det_ids)
  {
    GEMDetId p_id(id);
    GEMDetId superch_id(p_id.region(), p_id.ring(), p_id.station(), 0, p_id.chamber(), 0);

    const auto& hit_co_pads = muonHitMatcher_->hitCoPadsInDetId(id);
    const auto& co_pads_in_det = co_pads.get(superch_id);

    if (verboseCoPad_)
    {
      cout<<"matching CoPads in detid "<< superch_id << std::endl;
      cout<<"checkcopads from gemhits"<<hit_co_pads.size()<<" from copad collection "<<std::distance(co_pads_in_det.first, co_pads_in_det.second)<<" hit_pads: ";
      copy(hit_co_pads.begin(), hit_co_pads.end(), ostream_iterator<int>(cout," "));
      cout<<endl;
    }

    for (auto pad = co_pads_in_det.first; pad != co_pads_in_det.second; ++pad)
      {
      // to match simtrack to GEMCoPad, check the pads within the copad!
      bool matchL1 = false;
      GEMDetId gemL1_id(p_id.region(), p_id.ring(), p_id.station(), 1, p_id.chamber(), 0);
      if (verboseCoPad_) cout<<"CoPad: chp "<<*pad<<endl;
      for (const auto& p: padsInChamber(gemL1_id.rawId())) {
        if (p==pad->first()){
          matchL1 = true;
          break;
        }
      }

      bool matchL2 = false;
      GEMDetId gemL2_id(p_id.region(), p_id.ring(), p_id.station(), 2, p_id.chamber(), 0);
      for (const auto& p: padsInChamber(gemL2_id.rawId())) {
        if (p==pad->second()){
          matchL2 = true;
          break;
        }
      }

      if (matchL1 and matchL2) {
        if (verboseCoPad_) cout<<"CoPad: was matched! "<<endl;
        superchamber_to_copads_[ superch_id() ].push_back(*pad);
      }
    }
  }
}




std::set<unsigned int>
GEMDigiMatcher::detIdsDigi(int gem_type) const
{
  return selectDetIds(detid_to_digis_, gem_type);
}


std::set<unsigned int>
GEMDigiMatcher::detIdsPad(int gem_type) const
{
  return selectDetIds(detid_to_pads_, gem_type);
}


std::set<unsigned int>
GEMDigiMatcher::chamberIdsDigi(int gem_type) const
{
  return selectDetIds(chamber_to_digis_, gem_type);
}


std::set<unsigned int>
GEMDigiMatcher::chamberIdsPad(int gem_type) const
{
  return selectDetIds(chamber_to_pads_, gem_type);
}


std::set<unsigned int>
GEMDigiMatcher::superChamberIdsDigi(int gem_type) const
{
  return selectDetIds(superchamber_to_digis_, gem_type);
}


std::set<unsigned int>
GEMDigiMatcher::superChamberIdsPad(int gem_type) const
{
  return selectDetIds(superchamber_to_pads_, gem_type);
}


std::set<unsigned int>
GEMDigiMatcher::superChamberIdsCoPad(int gem_type) const
{
  return selectDetIds(superchamber_to_copads_, gem_type);
}


const GEMDigiContainer&
GEMDigiMatcher::digisInDetId(unsigned int detid) const
{
  if (detid_to_digis_.find(detid) == detid_to_digis_.end()) return no_gem_digis_;
  return detid_to_digis_.at(detid);
}


const GEMDigiContainer&
GEMDigiMatcher::digisInChamber(unsigned int detid) const
{
  if (chamber_to_digis_.find(detid) == chamber_to_digis_.end()) return no_gem_digis_;
  return chamber_to_digis_.at(detid);
}


const GEMDigiContainer&
GEMDigiMatcher::digisInSuperChamber(unsigned int detid) const
{
  if (superchamber_to_digis_.find(detid) == superchamber_to_digis_.end()) return no_gem_digis_;
  return superchamber_to_digis_.at(detid);
}


const GEMPadDigiContainer&
GEMDigiMatcher::padsInDetId(unsigned int detid) const
{
  if (detid_to_pads_.find(detid) == detid_to_pads_.end()) return no_gem_pads_;
  return detid_to_pads_.at(detid);
}


const GEMPadDigiContainer&
GEMDigiMatcher::padsInChamber(unsigned int detid) const
{
  if (chamber_to_pads_.find(detid) == chamber_to_pads_.end()) return no_gem_pads_;
  return chamber_to_pads_.at(detid);
}


const GEMPadDigiContainer&
GEMDigiMatcher::padsInSuperChamber(unsigned int detid) const
{
  if (superchamber_to_pads_.find(detid) == superchamber_to_pads_.end()) return no_gem_pads_;
  return superchamber_to_pads_.at(detid);
}


const GEMCoPadDigiContainer&
GEMDigiMatcher::coPadsInSuperChamber(unsigned int detid) const
{
  if (superchamber_to_copads_.find(detid) == superchamber_to_copads_.end()) return no_gem_copads_;
  return superchamber_to_copads_.at(detid);
}


int
GEMDigiMatcher::nLayersWithDigisInSuperChamber(unsigned int detid) const
{
  set<int> layers;
  GEMDetId sch_id(detid);
  for (int iLayer=1; iLayer<=2; iLayer++){
    GEMDetId ch_id(sch_id.region(),sch_id.ring(), sch_id.station(), iLayer, sch_id.chamber(), 0);
    // get the digis in this chamber
    const auto& digis = digisInChamber(ch_id.rawId());
    // at least one digi in this layer!
    if (digis.size()>0){
      layers.insert(iLayer);
    }
  }
  return layers.size();
}


int
GEMDigiMatcher::nLayersWithPadsInSuperChamber(unsigned int detid) const
{
  set<int> layers;
  GEMDetId sch_id(detid);
  for (int iLayer=1; iLayer<=2; iLayer++){
    GEMDetId ch_id(sch_id.region(),sch_id.ring(), sch_id.station(), iLayer, sch_id.chamber(), 0);
    // get the pads in this chamber
    const auto& pads = padsInChamber(ch_id.rawId());
    // at least one digi in this layer!
    if (pads.size()>0){
      layers.insert(iLayer);
    }
  }
  return layers.size();
}


int
GEMDigiMatcher::nPads() const
{
  int n = 0;
  const auto& ids = superChamberIdsPad();
  for (const auto& id: ids)
  {
    n += padsInSuperChamber(id).size();
  }
  return n;
}


int
GEMDigiMatcher::nCoPads() const
{
  int n = 0;
  const auto& ids = superChamberIdsCoPad();
  for (const auto& id: ids)
  {
    n += coPadsInSuperChamber(id).size();
  }
  return n;
}


std::set<int>
GEMDigiMatcher::stripNumbersInDetId(unsigned int detid) const
{
  set<int> result;
  const auto& digis = digisInDetId(detid);
  for (const auto& d: digis)
  {
    result.insert( d.strip() );
  }
  return result;
}


std::set<int>
GEMDigiMatcher::padNumbersInDetId(unsigned int detid) const
{
  set<int> result;
  const auto& digis = padsInDetId(detid);
  for (const auto& d: digis)
  {
    result.insert( d.pad() );
  }
  return result;
}


std::set<int>
GEMDigiMatcher::partitionNumbers() const
{
  std::set<int> result;

  const auto& detids = detIdsDigi();
  for (const auto& id: detids)
  {
    const GEMDetId& idd(id);
    result.insert( idd.roll() );
  }
  return result;
}


std::set<int>
GEMDigiMatcher::partitionNumbersWithCoPads() const
{
  std::set<int> result;

  const auto& detids = superChamberIdsCoPad();
  for (const auto& id: detids)
  {
    const GEMDetId& idd(id);
    result.insert( idd.roll() );
  }
  return result;
}

GlobalPoint
GEMDigiMatcher::getGlobalPointDigi(unsigned int rawId, const GEMDigi& d) const
{
  GEMDetId gem_id(rawId);
  const LocalPoint& gem_lp = gemGeometry_->etaPartition(gem_id)->centreOfStrip(d.strip());
  const GlobalPoint& gem_gp = gemGeometry_->idToDet(gem_id)->surface().toGlobal(gem_lp);
  return gem_gp;
}

GlobalPoint
GEMDigiMatcher::getGlobalPointPad(unsigned int rawId, const GEMPadDigi& tp) const
{
  GEMDetId gem_id(rawId);
  const LocalPoint& gem_lp = gemGeometry_->etaPartition(gem_id)->centreOfPad(tp.pad());
  const GlobalPoint& gem_gp = gemGeometry_->idToDet(gem_id)->surface().toGlobal(gem_lp);
  return gem_gp;
}
