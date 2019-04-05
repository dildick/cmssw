#include "Validation/MuonME0Validation/interface/ME0DigiMatcher.h"

using namespace std;

ME0DigiMatcher::ME0DigiMatcher(edm::ParameterSet const& pset, edm::ConsumesCollector && iC)
{
  const auto& me0Digi= pset.getParameterSet("me0DigiPreReco");
  minBX_ = me0Digi.getParameter<int>("minBX");
  maxBX_ = me0Digi.getParameter<int>("maxBX");
  matchDeltaStrip_ = me0Digi.getParameter<int>("matchDeltaStrip");
  verboseDigi_ = me0Digi.getParameter<int>("verbose");

  // make a new simhits matcher
  muonSimHitMatcher_.reset(new ME0SimHitMatcher(pset, std::move(iC)));

  me0DigiToken_ = iC.consumes<ME0DigiPreRecoCollection>(me0Digi.getParameter<edm::InputTag>("inputTag"));
}

// initialize the event
void ME0DigiMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  muonSimHitMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(me0DigiToken_, me0DigisH_);

  iSetup.get<MuonGeometryRecord>().get(me0_geom_);
  if (me0_geom_.isValid()) {
    me0Geometry_ = &*me0_geom_;
  } else {
    std::cout << "+++ Info: ME0 geometry is unavailable. +++\n";
  }
}

// do the matching
void ME0DigiMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match simhits first
  muonSimHitMatcher_->match(t,v);

  // get the digi collections
  const ME0DigiPreRecoCollection& me0Digis = *me0DigisH_.product();

  // now match the digis
  matchPreRecoDigisToSimTrack(me0Digis);
}

void
ME0DigiMatcher::matchPreRecoDigisToSimTrack(const ME0DigiPreRecoCollection& digis)
{
  const auto& det_ids = muonSimHitMatcher_->detIds();
  for (const auto& id: det_ids)
  {
    ME0DetId p_id(id);

    const auto& digis_in_det = digis.get(ME0DetId(id));

    for (auto d = digis_in_det.first; d != digis_in_det.second; ++d)
    {
      // check that the digi is within BX range
     // if (d->tof() < minBX_ || d->tof() > maxBX_) continue;

      // check that the pdgid is 13 for muon!
      if (std::abs(d->pdgid()) != 13 and std::abs(d->pdgid()) != 11) continue;
      //if (std::abs(d->pdgid()) != 13) continue;

      if (verboseDigi_) cout<<"ME0 digi "<<p_id<<" "<<*d<<endl;

      bool match = false;

      for (const auto& hit: muonSimHitMatcher_->hitsInDetId(id)){
	if (verboseDigi_)
	    cout << "\tCandidate ME0 simhit " << hit << " local (x,y)= " << hit.localPosition().x() << " " << hit.localPosition().y()<< " tof "<< hit.tof() << " pdgid " << hit.particleType() << endl;
        // check that the digi position matches a simhit position (within 5 sigma)
        //if (d->x() - 5 * d->ex() < hit.localPosition().x() and
        //    d->x() + 5 * d->ex() > hit.localPosition().x() and
        //    d->y() - 5 * d->ey() < hit.localPosition().y() and
        //    d->y() + 5 * d->ey() > hit.localPosition().y() ) {
	if (std::fabs(d->tof() - hit.tof()) > 50) continue;
  // check that the digi position matches a simhit position (within 3 sigma)
  if (std::fabs(d->x() - hit.localPosition().x()) < .5  and
      std::fabs(d->y() - hit.localPosition().y()) < 2.5*6 ){
          match = true;
          //cout << "\t...matches this digi!" << endl;
          break;
        }
      }

      if (match) {
        if (verboseDigi_) cout<<"-->Digi was matched"<<endl;
        detid_to_digis_[id].push_back(*d);
        chamber_to_digis_[ p_id.layerId().rawId() ].push_back(*d);
        superchamber_to_digis_[ p_id.chamberId().rawId() ].push_back(*d);
      }
    }
  }
}


std::set<unsigned int>
ME0DigiMatcher::detIds() const
{
  std::set<unsigned int> result;
  for (const auto& p: detid_to_digis_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
ME0DigiMatcher::chamberIds() const
{
  std::set<unsigned int> result;
  for (const auto& p: chamber_to_digis_) result.insert(p.first);
  return result;
}

std::set<unsigned int>
ME0DigiMatcher::superChamberIds() const
{
  std::set<unsigned int> result;
  for (const auto& p: superchamber_to_digis_) result.insert(p.first);
  return result;
}


const ME0DigiPreRecoContainer&
ME0DigiMatcher::digisInDetId(unsigned int detid) const
{
  if (detid_to_digis_.find(detid) == detid_to_digis_.end()) return no_me0_digis_;
  return detid_to_digis_.at(detid);
}

const ME0DigiPreRecoContainer&
ME0DigiMatcher::digisInChamber(unsigned int detid) const
{
  if (chamber_to_digis_.find(detid) == chamber_to_digis_.end()) return no_me0_digis_;
  return chamber_to_digis_.at(detid);
}

const ME0DigiPreRecoContainer&
ME0DigiMatcher::digisInSuperChamber(unsigned int detid) const
{
  if (superchamber_to_digis_.find(detid) == superchamber_to_digis_.end()) return no_me0_digis_;
  return superchamber_to_digis_.at(detid);
}

int
ME0DigiMatcher::nLayersWithDigisInSuperChamber(unsigned int detid) const
{
  set<int> layers;
  ME0DetId sch_id(detid);
  for (int iLayer=1; iLayer<=6; iLayer++){
    ME0DetId ch_id(sch_id.region(), iLayer, sch_id.chamber(), 0);
    // get the digis in this chamber
    const auto& digis = digisInChamber(ch_id.rawId());
    // at least one digi in this layer!
    if (digis.size()>0){
      layers.insert(iLayer);
    }
  }
  return layers.size();
}


std::set<int>
ME0DigiMatcher::stripNumbersInDetId(unsigned int detid) const
{
  set<int> result;
  // const auto& digis = digisInDetId(detid);
  // for (auto& d: digis)
  // {
  //   result.insert( digi_channel(d) );
  // }
  return result;
}

std::set<int>
ME0DigiMatcher::partitionNumbers() const
{
  std::set<int> result;

  const auto& detids = detIds();
  for (const auto& id: detids)
  {
    ME0DetId idd(id);
    result.insert( idd.roll() );
  }
  return result;
}
