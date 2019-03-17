
#include "Validation/MuonCSCDigis/interface/CSCDigiMatcher.h"

using namespace std;

CSCDigiMatcher::CSCDigiMatcher(const edm::Event&, const edm::EventSetup&,
                               edm::ParameterSet const& iPS, edm::ConsumesCollector && iC,
                               const SimHitMatcher& sh)
{
  const auto& wireDigi = cfg.getParameterSet("cscWireDigi");
  verboseWG_ = wireDigi.getParameter<int>("verbose");
  minBXWire_ = wireDigi.getParameter<int>("minBX");
  maxBXWire_ = wireDigi.getParameter<int>("maxBX");
  matchDeltaWG_ = wireDigi.getParameter<int>("matchDeltaWG");

  const auto& cscComparatorDigi = cfg.getParameterSet("cscComparatorDigi");
  verboseWG_ = cscComparatorDigi.getParameter<int>("verbose");
  minBXComparator_ = cscComparatorDigi.getParameter<int>("minBX");
  maxBXComparator_ = cscComparatorDigi.getParameter<int>("maxBX");
  matchDeltaComparator_ = cscComparatorDigi.getParameter<int>("matchDeltaWG");

  const auto& cscStripDigi = cfg.getParameterSet("cscStripDigi");
  verboseStrip_ = cscStripDigi.getParameter<int>("verbose");
  minBXComp_ = cscStripDigi.getParameter<int>("minBX");
  maxBXComp_ = cscStripDigi.getParameter<int>("maxBX");
  matchDeltaStrip_ = cscStripDigi.getParameter<int>("matchDeltaStrip");

  const edm::EDGetTokenT<CSCComparatorDigiCollection>& cscComparatorDigiInput_;
  const edm::EDGetTokenT<CSCStripDigiCollection>& cscStripDigiInput_;
  const edm::EDGetTokenT<CSCWireDigiCollection>& wireDigiInput_;

  edm::Handle<CSCComparatorDigiCollection> comp_digis;
  iEvent.getByToken(cscComparatorDigiInput_, comp_digis);
  matchComparatorsToSimTrack(*comp_digis.product());

  edm::Handle<CSCStripDigiCollection> strip_digis;
  iEvent.getByToken(cscStripDigiInput_, strip_digis);
  matchStripsToSimTrack(*strip_digis.product());

  edm::Handle<CSCWireDigiCollection> wire_digis;
  iEvent.getByToken(wireDigiInput_, wire_digis);
  matchWiresToSimTrack(*wire_digis.product());
}


CSCDigiMatcher::~CSCDigiMatcher() {}


void
CSCDigiMatcher::matchComparatorsToSimTrack(const CSCComparatorDigiCollection& comparators)
{

 for (auto detUnitIt= comparators.begin(); detUnitIt!= comparators.end(); ++detUnitIt){
     const CSCDetId& id = (*detUnitIt).first;
     const auto& range =(*detUnitIt).second;
     for (auto digiIt =  range.first; digiIt!=range.second; ++digiIt){
       if (id.station() == 1 and (id.ring() == 1 or id.ring() ==4 ))
         if (verboseStrip_) cout <<"CSCid "<< id <<" Comparator digi (strip, comparator, Tbin ) "<< (*digiIt) << endl;
     }
 }

  const auto& det_ids = simhit_matcher_->detIdsCSC(0);
  for (const auto& id: det_ids)
  {
    CSCDetId layer_id(id);

    const auto& hit_strips = simhit_matcher_->hitStripsInDetId(id, matchDeltaStrip_);
    if (verboseStrip_)
    {
      cout<<"hit_strips_fat, CSCid " << layer_id <<" ";
      copy(hit_strips.begin(), hit_strips.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }

    const auto& comp_digis_in_det = comparators.get(layer_id);
    for (auto c = comp_digis_in_det.first; c != comp_digis_in_det.second; ++c)
    {
      if (verboseStrip_) cout<<"sdigi "<<layer_id<<" (strip, comparator, Tbin ) "<<*c<<endl;

      // check that the first BX for this digi wasn't too early or too late
      if (c->getTimeBin() < minBXCSCComp_ || c->getTimeBin() > maxBXCSCComp_) continue;

      int strip = c->getStrip(); // strips are counted from 1
      // check that it matches a strip that was hit by SimHits from our track
      if (hit_strips.find(strip) == hit_strips.end()) continue;

      if (verboseStrip_) cout<< "Matched strip "<< *c << endl;
      detid_to_halfstrips_[id].push_back(*c);
      chamber_to_halfstrips_[ layer_id.chamberId().rawId() ].push_back(*c);
    }
  }
}


void
CSCDigiMatcher::matchStripsToSimTrack(const CSCStripDigiCollection& strips)
{

 for (auto detUnitIt= strips.begin(); detUnitIt!= strips.end(); ++detUnitIt){
     const CSCDetId& id = (*detUnitIt).first;
     const auto& range =(*detUnitIt).second;
     for (auto digiIt =  range.first; digiIt!=range.second; ++digiIt){
       if (id.station() == 1 and (id.ring() == 1 or id.ring() ==4 ))
         if (verboseStrip_) cout <<"CSCid "<< id <<" Strip digi (strip, strip, Tbin ) "<< (*digiIt) << endl;
     }
 }

  const auto& det_ids = simhit_matcher_->detIdsCSC(0);
  for (const auto& id: det_ids)
  {
    CSCDetId layer_id(id);

    const auto& hit_strips = simhit_matcher_->hitStripsInDetId(id, matchDeltaStrip_);
    if (verboseStrip_)
    {
      cout<<"hit_strips_fat, CSCid " << layer_id <<" ";
      copy(hit_strips.begin(), hit_strips.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }

    const auto& strip_digis_in_det = strips.get(layer_id);
    for (auto c = strip_digis_in_det.first; c != strip_digis_in_det.second; ++c)
    {
      if (verboseStrip_) cout<<"sdigi "<<layer_id<<" (strip, Tbin ) "<<*c<<endl;

      int strip = c->getStrip(); // strips are counted from 1
      // check that it matches a strip that was hit by SimHits from our track
      if (hit_strips.find(strip) == hit_strips.end()) continue;

      if (verboseStrip_) cout<< "Matched strip "<< *c << endl;
      detid_to_strips_[id].push_back(*c);
      chamber_to_strips_[ layer_id.chamberId().rawId() ].push_back(*c);
    }
  }
}


void
CSCDigiMatcher::matchWiresToSimTrack(const CSCWireDigiCollection& wires)
{
  const auto& det_ids = simhit_matcher_->detIdsCSC(0);
  for (const auto& id: det_ids)
  {
    CSCDetId layer_id(id);

    const auto& hit_wires = simhit_matcher_->hitWiregroupsInDetId(id, matchDeltaWG_);
    if (verboseWG_)
    {
      cout<<"hit_wires ";
      copy(hit_wires.begin(), hit_wires.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }

    const auto& wire_digis_in_det = wires.get(layer_id);
    for (auto w = wire_digis_in_det.first; w != wire_digis_in_det.second; ++w)
    {
      // check that the first BX for this digi wasn't too early or too late
      if (w->getTimeBin() < minBXCSCWire_ || w->getTimeBin() > maxBXCSCWire_) continue;

      int wg = w->getWireGroup(); // wiregroups are counted from 1
      // check that it matches a strip that was hit by SimHits from our track
      if (hit_wires.find(wg) == hit_wires.end()) continue;

      if (verboseStrip_) cout<< "Matched wire digi "<< *w << endl;
      detid_to_wires_[id].push_back(*w);
      chamber_to_wires_[ layer_id.chamberId().rawId() ].push_back(*w);
    }
  }
}

std::set<unsigned int>
CSCDigiMatcher::detIdsComparator(int csc_type) const
{
  return selectDetIds(detid_to_comparators_, csc_type);
}


std::set<unsigned int>
CSCDigiMatcher::detIdsWire(int csc_type) const
{
  return selectDetIds(detid_to_wires_, csc_type);
}


std::set<unsigned int>
CSCDigiMatcher::chamberIdsComparator(int csc_type) const
{
  return selectDetIds(chamber_to_comparators_, csc_type);
}


std::set<unsigned int>
CSCDigiMatcher::chamberIdsWire(int csc_type) const
{
  return selectDetIds(chamber_to_wires_, csc_type);
}


const CSCComparatorDigiContainer&
CSCDigiMatcher::comparatorDigisInDetId(unsigned int detid) const
{
  if (detid_to_comparators_.find(detid) == detid_to_comparators_.end()) return no__strips_;
  return detid_to_comparators_.at(detid);
}


const CSCComparatorDigiContainer&
CSCDigiMatcher::comparatorDigisInChamber(unsigned int detid) const
{
  if (chamber_to_comparators_.find(detid) == chamber_to_comparators_.end()) return no__strips_;
  return chamber_to_comparators_.at(detid);
}


const CSCStripDigiContainer&
CSCDigiMatcher::stripDigisInDetId(unsigned int detid) const
{
  if (detid_to_strips_.find(detid) == detid_to_strips_.end()) return no__strips_;
  return detid_to_strips_.at(detid);
}


const CSCStripDigiContainer&
CSCDigiMatcher::stripDigisInChamber(unsigned int detid) const
{
  if (chamber_to_strips_.find(detid) == chamber_to_strips_.end()) return no__strips_;
  return chamber_to_strips_.at(detid);
}


const CSCWireDigiContainer&
CSCDigiMatcher::wireDigisInDetId(unsigned int detid) const
{
  if (detid_to_wires_.find(detid) == detid_to_wires_.end()) return no__wires_;
  return detid_to_wires_.at(detid);
}


const CSCWireDigiContainer&
CSCDigiMatcher::wireDigisInChamber(unsigned int detid) const
{
  if (chamber_to_wires_.find(detid) == chamber_to_wires_.end()) return no__wires_;
  return chamber_to_wires_.at(detid);
}


int
CSCDigiMatcher::nLayersWithComparatorInChamber(unsigned int detid) const
{
  set<int> layers_with_hits;
  const auto& digis = comparatorDigisInChamber(detid);
  for (const auto& d: digis)
  {
    CSCDetId idd(digi_id(d));
    layers_with_hits.insert(idd.layer());
  }
  return layers_with_hits.size();
}


int
CSCDigiMatcher::nLayersWithStripInChamber(unsigned int detid) const
{
  set<int> layers_with_hits;
  const auto& digis = stripDigisInChamber(detid);
  for (const auto& d: digis)
  {
    CSCDetId idd(digi_id(d));
    layers_with_hits.insert(idd.layer());
  }
  return layers_with_hits.size();
}


int
CSCDigiMatcher::nLayersWithWireInChamber(unsigned int detid) const
{
  set<int> layers_with_hits;
  const auto& digis = wireDigisInChamber(detid);
  for (const auto& d: digis)
  {
    CSCDetId idd(digi_id(d));
    layers_with_hits.insert(idd.layer());
  }
  return layers_with_hits.size();
}


int
CSCDigiMatcher::nCoincidenceComparatorChambers(int min_n_layers) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsComparator();
  for (const auto& id: chamber_ids)
  {
    if (nLayersWithComparatorInChamber(id) >= min_n_layers) result += 1;
  }
  return result;
}


int
CSCDigiMatcher::nCoincidenceStripChambers(int min_n_layers) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsStrip();
  for (const auto& id: chamber_ids)
  {
    if (nLayersWithStripInChamber(id) >= min_n_layers) result += 1;
  }
  return result;
}


int
CSCDigiMatcher::nCoincidenceWireChambers(int min_n_layers) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsWire();
  for (const auto& id: chamber_ids)
  {
    if (nLayersWithWireInChamber(id) >= min_n_layers) result += 1;
  }
  return result;
}


std::set<int>
CSCDigiMatcher::comparatorsInDetId(unsigned int detid) const
{
  set<int> result;
  const auto& digis = comparatorDigisInDetId(detid);
  for (const auto& d: digis)
  {
    result.insert( d.getHalfStrip() );
  }
  return result;
}


std::set<int>
CSCDigiMatcher::stripsInDetId(unsigned int detid) const
{
  set<int> result;
  const auto& digis = stripDigisInDetId(detid);
  for (const auto& d: digis)
  {
    result.insert( d.getStrip() );
  }
  return result;
}


std::set<int>
CSCDigiMatcher::wiregroupsInDetId(unsigned int detid) const
{
  set<int> result;
  const auto& digis = wireDigisInDetId(detid);
  for (const auto& d: digis)
  {
    result.insert( d.getKeyWG() );
  }
  return result;
}


std::set<int>
CSCDigiMatcher::stripsInChamber(unsigned int detid, int max_gap_to_fill) const
{
  set<int> result;
  const auto& digis = stripDigisInChamber(detid);
  for (const auto& d: digis)
  {
    result.insert( digi_channel(d) );
  }
  if (max_gap_to_fill > 0)
  {
    int prev = -111;
    for (const auto& s: result)
    {
      //cout<<"gap "<<s<<" - "<<prev<<" = "<<s - prev<<"  added 0";
      if (s - prev > 1 && s - prev - 1 <= max_gap_to_fill)
      {
        //int sz = result.size();
        for (int fill_s = prev+1; fill_s < s; ++fill_s) result.insert(fill_s);
        //cout<<result.size() - sz;
      }
      //cout<<" elems"<<endl;
      prev = s;
    }
  }

  return result;
}

std::set<int>
CSCDigiMatcher::wiregroupsInChamber(unsigned int detid, int max_gap_to_fill) const
{
  set<int> result;
  const auto& digis = wireDigisInChamber(detid);
  for (const auto& d: digis)
  {
    result.insert( digi_channel(d) );
  }
  if (max_gap_to_fill > 0)
  {
    int prev = -111;
    for (const auto& w: result)
    {
      if (w - prev > 1 && w - prev - 1 <= max_gap_to_fill)
      {
        for (int fill_w = prev+1; fill_w < w; ++fill_w) result.insert(fill_w);
      }
      prev = w;
    }
  }
  return result;
}
