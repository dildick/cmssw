#include "GEMCode/GEMValidation/interface/ME0DigiMatcher.h"
#include "GEMCode/GEMValidation/interface/SimHitMatcher.h"

using namespace std;
using namespace matching;

ME0DigiMatcher::ME0DigiMatcher(const SimHitMatcher& sh, const edm::EDGetTokenT<ME0DigiPreRecoCollection>& me0DigiInput_)
: DigiMatcher(sh)
{
  const auto& me0Digi_= conf().getParameter<edm::ParameterSet>("me0DigiPreReco");
  minBXME0_ = me0Digi_.getParameter<int>("minBX");
  maxBXME0_ = me0Digi_.getParameter<int>("maxBX");
  matchDeltaStrip_ = me0Digi_.getParameter<int>("matchDeltaStrip");
  verboseDigi_ = me0Digi_.getParameter<int>("verbose");
  runME0Digi_ = me0Digi_.getParameter<bool>("run");

  if (hasME0Geometry_) {
    edm::Handle<ME0DigiPreRecoCollection> me0_digis;
    if (gemvalidation::getByToken(me0DigiInput_, me0_digis, event()) and runME0Digi_) matchPreRecoDigisToSimTrack(*me0_digis.product());
    else if (verboseDigi_) std::cout <<"Not running matchPreRecoDigisToSimTrack "<< std::endl;
  }
}

ME0DigiMatcher::~ME0DigiMatcher()
{}


void
ME0DigiMatcher::matchPreRecoDigisToSimTrack(const ME0DigiPreRecoCollection& digis)
{
  const auto& det_ids = simhit_matcher_->detIdsME0();
  for (const auto& id: det_ids)
  {
    ME0DetId p_id(id);

    const auto& digis_in_det = digis.get(ME0DetId(id));

    for (auto d = digis_in_det.first; d != digis_in_det.second; ++d)
    {
      // check that the digi is within BX range
     // if (d->tof() < minBXME0_ || d->tof() > maxBXME0_) continue;

      // check that the pdgid is 13 for muon!
      if (std::abs(d->pdgid()) != 13 and std::abs(d->pdgid()) != 11) continue;
      //if (std::abs(d->pdgid()) != 13) continue;

      if (verboseDigi_) cout<<"ME0 digi "<<p_id<<" "<<*d<<endl;

      bool match = false;

      for (const auto& hit: simhit_matcher_->hitsInDetId(id)){
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
