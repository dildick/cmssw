#include "Validation/RPCRecHits/interface/RPCRecHitMatcher.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

using namespace std;

RPCRecHitMatcher::RPCRecHitMatcher(const edm::ParameterSet& pset, edm::ConsumesCollector && iC)
{
  const auto& recHit = pset.getParameterSet("rpcRecHit");
  minBX_ = recHit.getParameter<int>("minBX");
  maxBX_ = recHit.getParameter<int>("maxBX");
  verbose_ = recHit.getParameter<int>("verbose");

  // make a new digi matcher
  rpcDigiMatcher_.reset(new RPCDigiMatcher(pset, std::move(iC)));

  recHitToken_ = iC.consumes<RPCRecHitCollection>(recHit.getParameter<edm::InputTag>("inputTag"));
}

void RPCRecHitMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  rpcDigiMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(recHitToken_, recHitH_);

  iSetup.get<MuonGeometryRecord>().get(rpc_geom_);
  if (rpc_geom_.isValid()) {
    rpcGeometry_ = &*rpc_geom_;
  } else {
    std::cout << "+++ Info: RPC geometry is unavailable. +++\n";
  }
}

/// do the matching
void RPCRecHitMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match digis first
  rpcDigiMatcher_->match(t,v);

  // get the rechit collection
  const RPCRecHitCollection& recHits = *recHitH_.product();

  // now match the rechits
  matchRecHitsToSimTrack(recHits);
}

void
RPCRecHitMatcher::matchRecHitsToSimTrack(const RPCRecHitCollection& rechits)
{
  // get the matched ids with digis
  const auto& det_ids = rpcDigiMatcher_->detIdsDigi();

  // loop on those ids
  for (auto id: det_ids)
  {
    // now check the digis in this detid
    const auto& hit_digis = rpcDigiMatcher_->stripNumbersInDetId(id);
    if (verbose())
    {
      cout<<"hit_digis_fat ";
      copy(hit_digis.begin(), hit_digis.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }

    RPCDetId p_id(id);
    const auto& rechits_in_det = rechits.get(p_id);

    for (auto d = rechits_in_det.first; d != rechits_in_det.second; ++d)
    {
      if (verbose()) cout<<"recHit "<<p_id<<" "<<*d<<endl;

      // check that the rechit is within BX range
      if (d->BunchX() < minBX_ || d->BunchX() > maxBX_) continue;

      int firstStrip = d->firstClusterStrip();
      int cls = d->clusterSize();
      bool stripFound = false;

      // check that it matches a strip that was hit by digis from our track
      for(int i = firstStrip; i < (firstStrip + cls); i++){

        if (hit_digis.find(i) != hit_digis.end()) stripFound = true;
        //std::cout<<i<<" "<<firstStrip<<" "<<cls<<" "<<stripFound<<std::endl;
      }

      // this rechit did not correspond with any previously matched digi
      if (!stripFound) continue;
      if (verbose()) cout<<"oki"<<endl;

      detid_to_recHits_[id].push_back(*d);
      chamber_to_recHits_[ p_id.chamberId().rawId() ].push_back(*d);
    }
  }
}


std::set<unsigned int>
RPCRecHitMatcher::detIds() const
{
  std::set<unsigned int> result;
  for (const auto& p: detid_to_recHits_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
RPCRecHitMatcher::chamberIds() const
{
  std::set<unsigned int> result;
  for (const auto& p: chamber_to_recHits_) result.insert(p.first);
  return result;
}

const RPCRecHitContainer&
RPCRecHitMatcher::recHitsInDetId(unsigned int detid) const
{
  if (detid_to_recHits_.find(detid) == detid_to_recHits_.end()) return no_recHits_;
  return detid_to_recHits_.at(detid);
}

const RPCRecHitContainer&
RPCRecHitMatcher::recHitsInChamber(unsigned int detid) const
{
  if (chamber_to_recHits_.find(detid) == chamber_to_recHits_.end()) return no_recHits_;
  return chamber_to_recHits_.at(detid);
}


const RPCRecHitContainer
RPCRecHitMatcher::recHits() const
{
  RPCRecHitContainer result;
  for (const auto& id: chamberIds()){
    const auto& rechitsInChamber(recHitsInChamber(id));
    result.insert(result.end(), rechitsInChamber.begin(), rechitsInChamber.end());
  }
  return result;
}

std::set<int>
RPCRecHitMatcher::stripNumbersInDetId(unsigned int detid) const
{
  set<int> result;
  for (const auto& d: recHitsInDetId(detid))
  {
    // loop on all strips hit in this rechit
    for (int iStrip = d.firstClusterStrip();
         iStrip < d.firstClusterStrip() + d.clusterSize(); ++iStrip) {
      result.insert( iStrip );
    }
  }
  return result;
}

std::set<int>
RPCRecHitMatcher::partitionNumbers() const
{
  std::set<int> result;

  for (auto id: detIds())
  {
    RPCDetId idd(id);
    result.insert( idd.roll() );
  }
  return result;
}

GlobalPoint
RPCRecHitMatcher::recHitPosition(const RPCRecHit& rechit) const
{
  const RPCDetId& idd = rechit.rpcId();
  const LocalPoint& lp = rechit.localPosition();
  return rpcGeometry_->idToDet(idd)->surface().toGlobal(lp);
}


GlobalPoint
RPCRecHitMatcher::recHitMeanPosition(const RPCRecHitContainer& rechit) const
{
  GlobalPoint point_zero;
  if (rechit.empty()) return point_zero; // point "zero"

  float sumx, sumy, sumz;
  sumx = sumy = sumz = 0.f;
  size_t n = 0;
  for (const auto& d: rechit)
  {
    GlobalPoint gp = recHitPosition(d);
    if (gp == point_zero) continue;

    sumx += gp.x();
    sumy += gp.y();
    sumz += gp.z();
    ++n;
  }
  if (n == 0) return GlobalPoint();
  return GlobalPoint(sumx/n, sumy/n, sumz/n);
}

bool
RPCRecHitMatcher::recHitInContainer(const RPCRecHit& rh, const RPCRecHitContainer& c) const
{
  bool isSame = false;
  for (const auto& thisRH: c) if (areRPCRecHitSame(thisRH,rh)) isSame = true;
  return isSame;
}


bool
RPCRecHitMatcher::isRPCRecHitMatched(const RPCRecHit& thisRh) const
{
  return recHitInContainer(thisRh, recHits());
}


bool
RPCRecHitMatcher::areRPCRecHitSame(const RPCRecHit& l, const RPCRecHit& r) const
{
  return l.localPosition()==r.localPosition() and l.BunchX()==r.BunchX();
}
