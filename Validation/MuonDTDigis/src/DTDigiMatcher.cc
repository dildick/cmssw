#include "Validation/MuonDTDigis/interface/DTDigiMatcher.h"
#include "DataFormats/MuonDetId/interface/DTWireId.h"

using namespace std;

DTDigiMatcher::DTDigiMatcher(const edm::ParameterSet& pset, edm::ConsumesCollector && iC)
{
  const auto& dtWire = pset.getParameterSet("dtWireDigi");
  minBXDT_ = dtWire.getParameter<int>("minBX");
  maxBXDT_ = dtWire.getParameter<int>("maxBX");
  matchDeltaWire_ = dtWire.getParameter<int>("matchDeltaWire");
  verboseWire_ = dtWire.getParameter<int>("verbose");

  const auto& dtThDigi = pset.getParameterSet("dtThDigi");
  minBXDT_ = dtThDigi.getParameter<int>("minBX");
  maxBXDT_ = dtThDigi.getParameter<int>("maxBX");
  matchDeltaThDigi_ = dtThDigi.getParameter<int>("matchDeltaThDigi");
  verboseThDigi_ = dtThDigi.getParameter<int>("verbose");

  const auto& dtPhDigi = pset.getParameterSet("dtPhDigi");
  minBXDT_ = dtPhDigi.getParameter<int>("minBX");
  maxBXDT_ = dtPhDigi.getParameter<int>("maxBX");
  matchDeltaPhDigi_ = dtPhDigi.getParameter<int>("matchDeltaPhDigi");
  verbosePhDigi_ = dtPhDigi.getParameter<int>("verbose");

  // make a new simhits matcher
  muonHitMatcher_.reset(new MuonHitMatcher(pset, std::move(iC)));

  dtWireToken_ = iC.consumes<DTDigiCollection>(dtWire.getParameter<edm::InputTag>("inputTag"));
  dtThDigiToken_ = iC.consumes<L1MuDTChambThContainer>(dtThDigi.getParameter<edm::InputTag>("inputTag"));
  dtPhDigiToken_ = iC.consumes<L1MuDTChambPhContainer>(dtPhDigi.getParameter<edm::InputTag>("inputTag"));
}

void DTDigiMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  muonHitMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(dtWireToken_, dtWiresH_);
  iEvent.getByToken(dtThDigiToken_, dtThDigisH_);
  iEvent.getByToken(dtPhDigiToken_, dtPhDigisH_);

  iSetup.get<MuonGeometryRecord>().get(dt_geom_);
  if (dt_geom_.isValid()) {
    dtGeometry_ = &*dt_geom_;
  } else {
    std::cout << "+++ Info: DT geometry is unavailable. +++\n";
  }
}

/// do the matching
void DTDigiMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match simhits first
  muonHitMatcher_->match(t,v);

  // get the digi collections
  const DTDigiCollection& dtWires = *dtWiresH_.product();
  const L1MuDTChambThContainer& dtThDigis = *dtThDigisH_.product();
  const L1MuDTChambPhContainer& dtPhDigis = *dtPhDigisH_.product();

  // now match the digis
  matchWiresToSimTrack(dtWires);
  matchThDigisToSimTrack(dtThDigis);
  matchPhDigisToSimTrack(dtPhDigis);
}

void
DTDigiMatcher::matchWiresToSimTrack(const DTDigiCollection& digis)
{
  const auto& det_ids = muonHitMatcher_->detIdsDT();
  for (const auto& id: det_ids)
  {
    const DTLayerId l_id(id);

    const auto& hit_wires = muonHitMatcher_->hitWiresInDTLayerId(l_id, matchDeltaWire_);
    if (verboseWire_)
    {
      cout<<"hit_wires_fat ";
      copy(hit_wires.begin(), hit_wires.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }
    // get digis in this layer
    const auto& digis_in_det = digis.get(l_id);

    for (auto d = digis_in_det.first; d != digis_in_det.second; ++d)
    {
      if (verboseWire_) cout<<"dt wire digi "<<l_id<<" "<<*d<<endl;
      // check that it matches a wire that was hit by SimHits from our track
      if (hit_wires.find(d->wire()) == hit_wires.end()) continue;
      if (verboseWire_) cout<<"oki"<<endl;

      /// Constructor from a layerId and a wire number
      const DTWireId w_id(l_id, d->wire());
      detid_to_digis_[w_id].push_back(*d);
      layer_to_digis_[l_id].push_back(*d);
      const DTSuperLayerId sl_id(l_id.superlayerId());
      superLayer_to_digis_[sl_id].push_back(*d);
      const DTChamberId c_id(sl_id.chamberId());
      chamber_to_digis_[c_id].push_back(*d);
    }
  }
}


std::set<unsigned int>
DTDigiMatcher::selectDetIds(const std::map<unsigned int, DTDigiContainer>& digis, int dt_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: digis)
  {
    const auto& id = p.first;
    if (dt_type > 0)
    {
      DTWireId detId(id);
      if (MuonHitHelper::toDTType(detId.wheel(), detId.station()) != dt_type) continue;
    }
    result.insert(p.first);
  }
  return result;
}


void DTDigiMatcher::matchThDigisToSimTrack(const L1MuDTChambThContainer& digis)
{
  for (const auto& d: *digis.getContainer()){

    DTChamberId detId(d.whNum(),d.scNum(),d.stNum());

    // get the corresponding wire digis
    chamber_to_th_digis_[detId].push_back(d);
  }
}

void DTDigiMatcher::matchPhDigisToSimTrack(const L1MuDTChambPhContainer& digis)
{
  for (const auto& d: *digis.getContainer()){

    DTChamberId detId(d.whNum(),d.scNum(),d.stNum());

    // get the corresponding wire digis
    chamber_to_ph_digis_[detId].push_back(d);
  }
}

std::set<unsigned int>
DTDigiMatcher::detIds(int dt_type) const
{
  return selectDetIds(detid_to_digis_, dt_type);
}


std::set<unsigned int>
DTDigiMatcher::layerIds(int dt_type) const
{
  return selectDetIds(layer_to_digis_, dt_type);
}


std::set<unsigned int>
DTDigiMatcher::superLayerIds(int dt_type) const
{
  return selectDetIds(superLayer_to_digis_, dt_type);
}


std::set<unsigned int>
DTDigiMatcher::chamberIds(int dt_type) const
{
  return selectDetIds(chamber_to_digis_, dt_type);
}


const DTDigiMatcher::DTDigiContainer&
DTDigiMatcher::digisInDetId(unsigned int detid) const
{
  if (detid_to_digis_.find(detid) == detid_to_digis_.end()) return no_dt_digis_;
  return detid_to_digis_.at(detid);
}


const DTDigiMatcher::DTDigiContainer&
DTDigiMatcher::digisInLayer(unsigned int detid) const
{
  if (layer_to_digis_.find(detid) == layer_to_digis_.end()) return no_dt_digis_;
  return layer_to_digis_.at(detid);
}


const DTDigiMatcher::DTDigiContainer&
DTDigiMatcher::digisInSuperLayer(unsigned int detid) const
{
  if (superLayer_to_digis_.find(detid) == superLayer_to_digis_.end()) return no_dt_digis_;
  return superLayer_to_digis_.at(detid);
}


const DTDigiMatcher::DTDigiContainer&
DTDigiMatcher::digisInChamber(unsigned int detid) const
{
  if (chamber_to_digis_.find(detid) == chamber_to_digis_.end()) return no_dt_digis_;
  return chamber_to_digis_.at(detid);
}


int
DTDigiMatcher::nLayersWithDigisInSuperLayer(unsigned int detId) const
{
  set<int> layers;
  for (const auto& l: dtGeometry_->superLayer(DTSuperLayerId(detId))->layers())
  {
    DTLayerId lid(l->id());
    if (digisInLayer(lid.rawId()).size()!=0)
      layers.insert(lid.layer());
  }
  return layers.size();
}


int
DTDigiMatcher::nSuperLayersWithDigisInChamber(unsigned int detId) const
{
  set<int> superLayers;
  for (const auto& sl: dtGeometry_->chamber(DTChamberId(detId))->superLayers())
  {
    DTSuperLayerId slid(sl->id());
    if (digisInSuperLayer(slid.rawId()).size()!=0)
      superLayers.insert(slid.superLayer());
  }
  return superLayers.size();
}


std::set<int>
DTDigiMatcher::wireNumbersInDetId(unsigned int detid) const
{
  set<int> result;
  for (const auto& d: digisInDetId(detid)) {
    result.insert( d.wire() );
  }
  return result;
}
