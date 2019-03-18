#include "Validation/MuonRPCDigis/interface/RPCDigiMatcher.h"

using namespace std;

RPCDigiMatcher::RPCDigiMatcher(const edm::ParameterSet& pset, edm::ConsumesCollector && iC)
{
  const auto& rpcDigi = pset.getParameterSet("rpcStripDigi");
  minRPCDigi_ = rpcDigi.getParameter<int>("minBX");
  maxRPCDigi_ = rpcDigi.getParameter<int>("maxBX");
  matchDeltaStrip_ = rpcDigi.getParameter<int>("matchDeltaStrip");
  verboseDigi_ = rpcDigi.getParameter<int>("verbose");

  // make a new simhits matcher
  muonHitMatcher_.reset(new MuonHitMatcher(pset, std::move(iC)));

  rpcDigiToken_ = iC.consumes<RPCDigiCollection>(rpcDigi.getParameter<edm::InputTag>("inputTag"));
}

void RPCDigiMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  muonHitMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(rpcDigiToken_, rpcDigisH_);

  iSetup.get<MuonGeometryRecord>().get(rpc_geom_);
  if (rpc_geom_.isValid()) {
    rpcGeometry_ = &*rpc_geom_;
  } else {
    std::cout << "+++ Info: RPC geometry is unavailable. +++\n";
  }
}

/// do the matching
void RPCDigiMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match simhits first
  muonHitMatcher_->match(t,v);

  // get the digi collections
  const RPCDigiCollection& rpcDigis = *rpcDigisH_.product();

  // now match the digis
  matchDigisToSimTrack(rpcDigis);
}


void
RPCDigiMatcher::matchDigisToSimTrack(const RPCDigiCollection& digis)
{
  if (verboseDigi_) cout << "Matching simtrack to RPC digis" << endl;
  const auto& det_ids = muonHitMatcher_->detIdsRPC();
  for (const auto& id: det_ids)
  {
    RPCDetId p_id(id);
    const auto& hit_strips = muonHitMatcher_->hitStripsInDetId(id, matchDeltaStrip_);
    if (verboseDigi_)
    {
      cout<<"hit_strips_fat ";
      copy(hit_strips.begin(), hit_strips.end(), ostream_iterator<int>(cout, " "));
      cout<<endl;
    }

    const auto& digis_in_det = digis.get(RPCDetId(id));

    for (auto d = digis_in_det.first; d != digis_in_det.second; ++d)
    {
      if (verboseDigi_) cout<<"RPCDigi "<<p_id<<" "<<*d<<endl;
      // check that the digi is within BX range
      if (d->bx() < minRPCDigi_ || d->bx() > maxRPCDigi_) continue;
      // check that it matches a strip that was hit by SimHits from our track
      if (hit_strips.find(d->strip()) == hit_strips.end()) continue;
      if (verboseDigi_) cout<<"...was matched!"<<endl;

      //std::cout <<" strip "<< d->strip()<<" 2-strip pad "<<(d->strip()+1)/2 << " bx "<< d->bx() << std::endl;
      detid_to_digis_[id].push_back(*d);
      chamber_to_digis_[ p_id.chamberId().rawId() ].push_back(*d);
    }
  }
}


std::set<unsigned int>
RPCDigiMatcher::detIdsDigi(int rpc_type) const
{
  return selectDetIds(detid_to_digis_, rpc_type);
}


std::set<unsigned int>
RPCDigiMatcher::chamberIdsDigi(int rpc_type) const
{
  return selectDetIds(chamber_to_digis_, rpc_type);
}


const RPCDigiContainer&
RPCDigiMatcher::digisInDetId(unsigned int detid) const
{
  if (detid_to_digis_.find(detid) == detid_to_digis_.end()) return no_rpc_digis_;
  return detid_to_digis_.at(detid);
}


const RPCDigiContainer&
RPCDigiMatcher::digisInChamber(unsigned int detid) const
{
  if (chamber_to_digis_.find(detid) == chamber_to_digis_.end()) return no_rpc_digis_;
  return chamber_to_digis_.at(detid);
}

int
RPCDigiMatcher::nDigis() const
{
  int n = 0;
  for (const auto& id: detIdsDigi())
  {
    n += digisInDetId(id).size();
  }
  return n;
}


std::set<int>
RPCDigiMatcher::stripNumbersInDetId(unsigned int detid) const
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
RPCDigiMatcher::partitionNumbers() const
{
  std::set<int> result;

  const auto& detids = detIdsDigi();
  for (const auto& id: detids)
  {
    const RPCDetId& idd(id);
    result.insert( idd.roll() );
  }
  return result;
}


GlobalPoint
RPCDigiMatcher::getGlobalPointDigi(unsigned int rawId, const RPCDigi& d) const
{
  RPCDetId rpc_id(rawId);
  const LocalPoint& rpc_lp = rpcGeometry_->roll(rpc_id)->centreOfStrip(d.strip());
  const GlobalPoint& rpc_gp = rpcGeometry_->idToDet(rpc_id)->surface().toGlobal(rpc_lp);
  return rpc_gp;
}
