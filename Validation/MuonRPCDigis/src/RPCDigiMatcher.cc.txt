#include "GEMCode/GEMValidation/interface/RPCDigiMatcher.h"
#include "GEMCode/GEMValidation/interface/SimHitMatcher.h"

using namespace std;
using namespace matching;


RPCDigiMatcher::RPCDigiMatcher(const SimHitMatcher& sh, const edm::EDGetTokenT<RPCDigiCollection> &rpcDigiInput_)
: DigiMatcher(sh)
{
  const auto& rpcDigi_= conf().getParameter<edm::ParameterSet>("rpcStripDigi");
  minBXRPC_ = rpcDigi_.getParameter<int>("minBX");
  maxBXRPC_ = rpcDigi_.getParameter<int>("maxBX");
  matchDeltaStrip_ = rpcDigi_.getParameter<int>("matchDeltaStrip");
  verboseDigi_ = rpcDigi_.getParameter<int>("verbose");
  runRPCDigi_ = rpcDigi_.getParameter<bool>("run");

  setVerbose(conf().getUntrackedParameter<int>("verboseRPCDigi", 0));

  if (hasRPCGeometry_) {
    edm::Handle<RPCDigiCollection> rpc_digis;
    if (gemvalidation::getByToken(rpcDigiInput_, rpc_digis, event())) if (runRPCDigi_) matchDigisToSimTrack(*rpc_digis.product());
  }
}

RPCDigiMatcher::~RPCDigiMatcher() {}

void
RPCDigiMatcher::matchDigisToSimTrack(const RPCDigiCollection& digis)
{
  if (verboseDigi_) cout << "Matching simtrack to RPC digis" << endl;
  const auto& det_ids = simhit_matcher_->detIdsRPC();
  for (const auto& id: det_ids)
  {
    RPCDetId p_id(id);

    const auto& hit_strips = simhit_matcher_->hitStripsInDetId(id, matchDeltaStrip_);
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
      if (d->bx() < minBXRPC_ || d->bx() > maxBXRPC_) continue;
      // check that it matches a strip that was hit by SimHits from our track
      if (hit_strips.find(d->strip()) == hit_strips.end()) continue;
      if (verboseDigi_) cout<<"...was matched!"<<endl;

      const auto& mydigi = make_digi(id, d->strip(), d->bx(), RPC_STRIP);
      detid_to_digis_[id].push_back(mydigi);
      chamber_to_digis_[p_id.chamberId().rawId()].push_back(mydigi);

      detid_to_rpcDigis_[id].push_back(*d);
      chamber_to_rpcDigis_[p_id.chamberId().rawId()].push_back(*d);
    }
  }
}


std::set<unsigned int>
RPCDigiMatcher::selectDetIds(const Id2DigiContainer &digis, int rpc_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: digis)
  {
    const auto& id = p.first;
    if (rpc_type > 0)
    {
      RPCDetId detId(id);
      if (gemvalidation::toRPCType(detId.region(), detId.station(), detId.ring()) != rpc_type) continue;
    }
    result.insert(p.first);
  }
  return result;
}

std::set<unsigned int>
RPCDigiMatcher::detIds(int rpc_type) const
{
  return selectDetIds(detid_to_digis_, rpc_type);
}

std::set<unsigned int>
RPCDigiMatcher::chamberIds(int rpc_type) const
{
  return selectDetIds(chamber_to_digis_, rpc_type);
}

const matching::DigiContainer&
RPCDigiMatcher::digisInDetId(unsigned int detid) const
{
  if (detid_to_digis_.find(detid) == detid_to_digis_.end()) return no_digis_;
  return detid_to_digis_.at(detid);
}

const matching::DigiContainer&
RPCDigiMatcher::digisInChamber(unsigned int detid) const  //use chamber raw id here
{
  if (chamber_to_digis_.find(detid) == chamber_to_digis_.end()) return no_digis_;
  return chamber_to_digis_.at(detid);
}

const RPCDigiMatcher::RPCDigiContainer&
RPCDigiMatcher::rpcDigisInDetId(unsigned int detid) const
{
  if (detid_to_rpcDigis_.find(detid) == detid_to_rpcDigis_.end()) return no_rpc_digis_;
  return detid_to_rpcDigis_.at(detid);
}

const RPCDigiMatcher::RPCDigiContainer&
RPCDigiMatcher::rpcDigisInChamber(unsigned int detid) const
{
  if (chamber_to_rpcDigis_.find(detid) == chamber_to_rpcDigis_.end()) return no_rpc_digis_;
  return chamber_to_rpcDigis_.at(detid);
}

int
RPCDigiMatcher::nStrips() const
{
  int n = 0;
  const auto& ids = detIds();
  for (const auto& id: ids)
  {
    n += digisInDetId(id).size();
  }
  return n;
}

std::set<int>
RPCDigiMatcher::stripsInDetId(unsigned int detid) const
{
  set<int> result;
  const auto& digis = digisInDetId(detid);
  for (const auto& d: digis)
  {
    result.insert( digi_channel(d) );
  }
  return result;
}

std::set<int>
RPCDigiMatcher::partitionNumbers() const
{
  std::set<int> result;

  const auto& detids = detIds();
  for (const auto& id: detids)
  {
    RPCDetId idd(id);
    result.insert( idd.roll() );
  }
  return result;
}

int
RPCDigiMatcher::extrapolateHsfromRPC(unsigned int id, int rpcstrip) const
{
  int result = -1 ;

  RPCDetId rpc_id(id);
  if (rpc_id.region() == 0) return result;
  int endcap = (rpc_id.region()>0 ? 1 : 2);
  int cscchamber = CSCTriggerNumbering::chamberFromTriggerLabels(rpc_id.sector(), 0, rpc_id.station(), rpc_id.subsector());
  cscchamber = (cscchamber==1? 18 : (cscchamber-1));// or cscchamber = (cscchamber+16)%18+1;
  CSCDetId csc_id(endcap, rpc_id.station(), rpc_id.ring(), cscchamber, 0);

  //std::cout <<"RPC det " << rpc_id <<"  CSC det " << csc_id << std::endl;
  const CSCChamber* cscChamber(getCSCGeometry()->chamber(csc_id));
  const CSCLayer* cscKeyLayer(cscChamber->layer(3));
  const CSCLayerGeometry* cscKeyLayerGeometry(cscKeyLayer->geometry());

  const RPCChamber* rpcChamber(getRPCGeometry()->chamber(rpc_id));
  const auto& rpcRoll(rpcChamber->roll(2));//any roll
  const int nStrips(rpcRoll->nstrips());
  if (rpcstrip > nStrips or rpcstrip < 0) return result;

  const LocalPoint& lpRPC(rpcRoll->centreOfStrip(rpcstrip));
  const GlobalPoint& gp(rpcRoll->toGlobal(lpRPC));
  const LocalPoint& lpCSC(cscKeyLayer->toLocal(gp));
  const float strip(cscKeyLayerGeometry->strip(lpCSC));
  // HS are wrapped-around
  result = (int) (strip - 0.25)/0.5;
  return result;
}




