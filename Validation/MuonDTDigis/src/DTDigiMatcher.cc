#include "GEMCode/GEMValidation/interface/DTDigiMatcher.h"
#include "GEMCode/GEMValidation/interface/SimHitMatcher.h"

using namespace std;
using namespace matching;

#include "DataFormats/MuonDetId/interface/DTWireId.h"

DTDigiMatcher::DTDigiMatcher(const SimHitMatcher& sh, const edm::EDGetTokenT<DTDigiCollection>& dtDigiInput_)
: DigiMatcher(sh)
{
  const auto& dtDigi_= conf().getParameter<edm::ParameterSet>("dtDigi");
  minBXDT_ = dtDigi_.getParameter<int>("minBX");
  maxBXDT_ = dtDigi_.getParameter<int>("maxBX");
  matchDeltaWire_ = dtDigi_.getParameter<int>("matchDeltaWire");
  verboseDigi_ = dtDigi_.getParameter<int>("verbose");
  runDTDigi_ = dtDigi_.getParameter<bool>("run");

  if (hasDTGeometry_) {
    edm::Handle<DTDigiCollection> dt_digis;
    if(gemvalidation::getByToken(dtDigiInput_, dt_digis, event())) if (runDTDigi_) matchDigisToSimTrack(*dt_digis.product());
  }
}

DTDigiMatcher::~DTDigiMatcher() {}


void
DTDigiMatcher::matchDigisToSimTrack(const DTDigiCollection& digis)
{
  const auto& det_ids = simhit_matcher_->detIdsDT();
  for (const auto& id: det_ids)
  {
    const DTLayerId l_id(id);
    const DTSuperLayerId sl_id(l_id.superlayerId());
    const DTChamberId c_id(sl_id.chamberId());

    const auto& hit_wires = simhit_matcher_->hitWiresInDTLayerId(l_id, matchDeltaWire_);
    if (verboseDigi_)
    {
      cout<<"hit_wires_fat ";
      copy(hit_wires.begin(), hit_wires.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }
    // get digis in this layer
    const auto& digis_in_det = digis.get(l_id);

    for (auto d = digis_in_det.first; d != digis_in_det.second; ++d)
    {
      if (verboseDigi_) cout<<"dt wire digi "<<l_id<<" "<<*d<<endl;
      // check that it matches a wire that was hit by SimHits from our track
      if (hit_wires.find(d->wire()) == hit_wires.end()) continue;
      if (verboseDigi_) cout<<"oki"<<endl;

      /// Constructor from a layerId and a wire number
      const DTWireId w_id(l_id, d->wire());

      detid_to_digis_[w_id].push_back(*d);
      layer_to_digis_[l_id].push_back(*d);
      superLayer_to_digis_[sl_id].push_back(*d);
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
      if (gemvalidation::toDTType(detId.wheel(), detId.station()) != dt_type) continue;
    }
    result.insert(p.first);
  }
  return result;
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
DTDigiMatcher::nTubesWithDigisInLayer(unsigned int detid) const
{
  set<int> tubes;
  // FIXME
  return tubes.size();
}

int
DTDigiMatcher::nLayersWithDigisInSuperLayer(unsigned int detId) const
{
  set<int> layers;
  for (const auto& l: getDTGeometry()->superLayer(DTSuperLayerId(detId))->layers())
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
  for (const auto& sl: getDTGeometry()->chamber(DTChamberId(detId))->superLayers())
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
  // for (const auto& d: digisInDetId(detid))
  // {
  //   result.insert( digi_channel(d) );
  // }
  return result;
}
