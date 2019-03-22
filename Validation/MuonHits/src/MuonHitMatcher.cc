#include "Validation/MuonHits/interface/MuonHitMatcher.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include <algorithm>
#include <iomanip>

using namespace std;


MuonHitMatcher::MuonHitMatcher(const edm::ParameterSet& ps, edm::ConsumesCollector && iC)
{
  const auto& simVertex = ps.getParameterSet("simVertex");
  const auto& simTrack = ps.getParameterSet("simTrack");
  verboseSimTrack_ = simTrack.getParameter<int>("verbose");

  const auto& gemSimHit_ = ps.getParameterSet("gemSimHit");
  verboseGEM_ = gemSimHit_.getParameter<int>("verbose");
  simMuOnlyGEM_ = gemSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsGEM_ = gemSimHit_.getParameter<bool>("discardEleHits");

  const auto& cscSimHit_= ps.getParameterSet("cscSimHit");
  verboseCSC_ = cscSimHit_.getParameter<int>("verbose");
  simMuOnlyCSC_ = cscSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsCSC_ = cscSimHit_.getParameter<bool>("discardEleHits");

  const auto& me0SimHit_ = ps.getParameterSet("me0SimHit");
  verboseME0_ = me0SimHit_.getParameter<int>("verbose");
  simMuOnlyME0_ = me0SimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsME0_ = me0SimHit_.getParameter<bool>("discardEleHits");

  const auto& rpcSimHit_ = ps.getParameterSet("rpcSimHit");
  verboseRPC_ = rpcSimHit_.getParameter<int>("verbose");
  simMuOnlyRPC_ = rpcSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsRPC_ = rpcSimHit_.getParameter<bool>("discardEleHits");

  const auto& dtSimHit_ = ps.getParameterSet("dtSimHit");
  verboseDT_ = dtSimHit_.getParameter<int>("verbose");
  simMuOnlyDT_ = dtSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsDT_ = dtSimHit_.getParameter<bool>("discardEleHits");

  verboseSimTrack_ = (verboseCSC_ || verboseGEM_ || verboseME0_ || verboseRPC_ || verboseDT_);

  simVertexInput_ = iC.consumes<edm::SimVertexContainer>(simVertex.getParameter<edm::InputTag>("inputTag"));
  simTrackInput_ = iC.consumes<edm::SimTrackContainer>(simTrack.getParameter<edm::InputTag>("inputTag"));
  gemSimHitInput_ = iC.consumes<edm::PSimHitContainer>(gemSimHit_.getParameter<edm::InputTag>("inputTag"));
  cscSimHitInput_ = iC.consumes<edm::PSimHitContainer>(cscSimHit_.getParameter<edm::InputTag>("inputTag"));
  me0SimHitInput_ = iC.consumes<edm::PSimHitContainer>(me0SimHit_.getParameter<edm::InputTag>("inputTag"));
  rpcSimHitInput_ = iC.consumes<edm::PSimHitContainer>(rpcSimHit_.getParameter<edm::InputTag>("inputTag"));
  dtSimHitInput_ = iC.consumes<edm::PSimHitContainer>(dtSimHit_.getParameter<edm::InputTag>("inputTag"));
}

/// initialize the event
void MuonHitMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  hasGEMGeometry_ = true;
  hasRPCGeometry_ = true;
  hasCSCGeometry_ = true;
  hasME0Geometry_ = true;
  hasDTGeometry_ = true;

  iSetup.get<MuonGeometryRecord>().get(gem_geom_);
  if (gem_geom_.isValid()) {
    gemGeometry_ = &*gem_geom_;
  } else {
    hasGEMGeometry_ = false;
    std::cout << "+++ Info: GEM geometry is unavailable. +++\n";
  }

  iSetup.get<MuonGeometryRecord>().get(me0_geom_);
  if (me0_geom_.isValid()) {
    me0Geometry_ = &*me0_geom_;
  } else {
    hasME0Geometry_ = false;
    std::cout << "+++ Info: ME0 geometry is unavailable. +++\n";
  }

  iSetup.get<MuonGeometryRecord>().get(rpc_geom_);
  if (rpc_geom_.isValid()) {
    rpcGeometry_ = &*rpc_geom_;
  } else {
    hasRPCGeometry_ = false;
    std::cout << "+++ Info: RPC geometry is unavailable. +++\n";
  }

  iSetup.get<MuonGeometryRecord>().get(dt_geom_);
  if (dt_geom_.isValid()) {
    dtGeometry_ = &*dt_geom_;
  } else {
    hasDTGeometry_ = false;
    std::cout << "+++ Info: DT geometry is unavailable. +++\n";
  }

  iSetup.get<MuonGeometryRecord>().get(csc_geom_);
  if (csc_geom_.isValid()) {
    cscGeometry_ = &*csc_geom_;
  } else {
    hasCSCGeometry_ = false;
    std::cout << "+++ Info: CSC geometry is unavailable. +++\n";
  }

  iEvent.getByToken(simTrackInput_, simTracksH_);
  iEvent.getByToken(simVertexInput_, simVerticesH_);
  iEvent.getByToken(dtSimHitInput_, dtHitsH_);
  iEvent.getByToken(rpcSimHitInput_, rpcHitsH_);
  iEvent.getByToken(me0SimHitInput_, me0HitsH_);
  iEvent.getByToken(gemSimHitInput_, gemHitsH_);
  iEvent.getByToken(cscSimHitInput_, cscHitsH_);
}

/// do the matching
void MuonHitMatcher::match(const SimTrack& track, const SimVertex& vertex)
{
  const edm::SimTrackContainer& simTracks = *simTracksH_.product();
  const edm::SimVertexContainer& simVertices = *simVerticesH_.product();

  const edm::PSimHitContainer& dtHits = *dtHitsH_.product();
  const edm::PSimHitContainer& rpcHits = *rpcHitsH_.product();
  const edm::PSimHitContainer& cscHits = *cscHitsH_.product();
  const edm::PSimHitContainer& gemHits = *gemHitsH_.product();
  const edm::PSimHitContainer& me0Hits = *me0HitsH_.product();

  // fill trkId2Index association:
  int no = 0;
  trkid_to_index_.clear();
  for (const auto& t: simTracks) {
    trkid_to_index_[t.trackId()] = no;
    no++;
  }

  vector<unsigned> track_ids = getIdsOfSimTrackShower(track.trackId(), simTracks, simVertices);
  if (verboseSimTrack_) {
    std::cout << "Printing track_ids" << std::endl;
    for (const auto& id: track_ids) std::cout << "id: " << id << std::endl;
  }

  if (hasCSCGeometry_) {

    matchCSCSimHitsToSimTrack(track_ids, cscHits);

    if (verboseCSC_) {
      cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedCSCSimHits "<<cscHits.size()<<endl;
      cout<<"detids CSC " << detIdsCSC(0).size()<<endl;

      for (const auto& id: detIdsCSC(0)) {
        const auto& csc_simhits = hitsInDetId(id);
        const auto& csc_simhits_gp = simHitsMeanPosition(csc_simhits);
        const auto& strips = hitStripsInDetId(id);
        CSCDetId cscid(id);
        if (cscid.station() == 1 and (cscid.ring() == 1 or cscid.ring() == 4)){
          cout<<"cscdetid "<<CSCDetId(id)<<": "<<csc_simhits.size()<<" "<<csc_simhits_gp.phi()<<" "<< csc_detid_to_hits_[id].size()<<endl;
          cout<<"nStrip "<<strips.size()<<endl;
          cout<<"strips : "; std::copy(strips.begin(), strips.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
        }
      }
    }
  }

  if (hasGEMGeometry_) {

    matchGEMSimHitsToSimTrack(track_ids, gemHits);

    if (verboseGEM_) {
      cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedGEMSimHits "<<gemHits.size()<<endl;
      cout << "detids GEM " << detIdsGEM().size() << endl;

      const auto& gem_ch_ids = detIdsGEM();
      for (const auto& id: gem_ch_ids) {
        const auto& gem_simhits = hitsInDetId(id);
        const auto& gem_simhits_gp = simHitsMeanPosition(gem_simhits);
        cout<<"gemchid "<<GEMDetId(id)<<": nHits "<<gem_simhits.size()<<" phi "<<gem_simhits_gp.phi()<<" nCh "<< gem_chamber_to_hits_[id].size()<<endl;
        const auto& strips = hitStripsInDetId(id);
        cout<<"nStrip "<<strips.size()<<endl;
        cout<<"strips : "; std::copy(strips.begin(), strips.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
      }
      const auto& gem_sch_ids = superChamberIdsGEM();
      for (const auto& id: gem_sch_ids) {
        const auto& gem_simhits = hitsInSuperChamber(id);
        const auto& gem_simhits_gp = simHitsMeanPosition(gem_simhits);
        cout<<"gemschid "<<GEMDetId(id)<<": "<<nCoincidencePadsWithHits() <<" | "<<gem_simhits.size()<<" "<<gem_simhits_gp.phi()<<" "<< gem_superchamber_to_hits_[id].size()<<endl;
      }
    }
  }


  if (hasME0Geometry_) {

    matchME0SimHitsToSimTrack(track_ids, me0Hits);

    if (verboseME0_) {
      cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" ME0SimHits "<<me0Hits.size()<<endl;
      cout << "detids ME0 " << detIdsME0().size() << endl;

      const auto& me0_ch_ids = detIdsME0();
      for (const auto& id: me0_ch_ids) {
        const auto& me0_simhits = hitsInChamber(id);
        const auto& me0_simhits_gp = simHitsMeanPosition(me0_simhits);
        cout<<"me0chid "<<ME0DetId(id)<<": nHits "<<me0_simhits.size()<<" phi "<<me0_simhits_gp.phi()<<" nCh "<< me0_chamber_to_hits_[id].size()<<endl;
        const auto& strips = hitStripsInDetId(id);
        cout<<"nStrip "<<strips.size()<<endl;
        cout<<"strips : "; std::copy(strips.begin(), strips.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
      }
    }
  }


  if (hasRPCGeometry_) {

    matchRPCSimHitsToSimTrack(track_ids, rpcHits);

    if (verboseRPC_) {
      cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedRPCSimHits "<<rpcHits.size()<<endl;
      cout << "detids RPC " << detIdsRPC().size() << endl;

      const auto& rpc_ch_ids = chamberIdsRPC();
      for (const auto& id: rpc_ch_ids) {
        const auto& rpc_simhits = hitsInChamber(id);
        const auto& rpc_simhits_gp = simHitsMeanPosition(rpc_simhits);
        cout<<"RPCDetId "<<RPCDetId(id)<<": nHits "<<rpc_simhits.size()<<" eta "<<rpc_simhits_gp.eta()<<" phi "<<rpc_simhits_gp.phi()<<" nCh "<< rpc_chamber_to_hits_[id].size()<<endl;
        const auto& strips = hitStripsInDetId(id);
        cout<<"nStrips "<<strips.size()<<endl;
        cout<<"strips : "; std::copy(strips.begin(), strips.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
      }
    }
  }


  if (hasDTGeometry_) {

    matchDTSimHitsToSimTrack(track_ids, dtHits);

    if (verboseDT_) {
      cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedDTSimHits "<<dtHits.size()<<endl;
      cout<<"detids DT " << detIdsDT().size()<<endl;

      const auto& dt_det_ids = detIdsDT();
      for (const auto& id: dt_det_ids) {
        const auto& dt_simhits = hitsInDetId(id);
        const auto& dt_simhits_gp = simHitsMeanPosition(dt_simhits);
        cout<<"DTWireId "<<DTWireId(id)<<": nHits "<<dt_simhits.size()<<" eta "<<dt_simhits_gp.eta()<<" phi "<<dt_simhits_gp.phi()<<" nCh "<< dt_chamber_to_hits_[id].size()<<endl;
        // only 1 wire per DT cell
        // const auto& wires = hitWiresInDTLayerId(id);
        // cout<<"nWires "<<wires.size()<<endl;
        // cout<<"wires : "; std::copy(wires.begin(), wires.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
      }
    }
  }
}

std::vector<unsigned int>
MuonHitMatcher::getIdsOfSimTrackShower(unsigned int initial_trk_id,
    const edm::SimTrackContainer & simTracks, const edm::SimVertexContainer & simVertices)
{
  vector<unsigned int> result;
  result.push_back(initial_trk_id);

  if (! (simMuOnlyGEM_ || simMuOnlyCSC_ || simMuOnlyDT_ || simMuOnlyME0_ || simMuOnlyRPC_) ) return result;

  for (const auto& t: simTracks)
  {
    SimTrack last_trk = t;
    //if (std::abs(t.type()) != 13) continue;
    bool is_child = 0;
    while (1)
    {
      if ( last_trk.noVertex() ) break;
      if ( simVertices[last_trk.vertIndex()].noParent() ) break;

      unsigned parentId = simVertices[last_trk.vertIndex()].parentIndex();
      if ( parentId == initial_trk_id )
      {
        is_child = 1;
        break;
      }

      const auto& association = trkid_to_index_.find( parentId );
      if ( association == trkid_to_index_.end() ) break;

      last_trk = simTracks[ association->second ];
    }
    if (is_child)
    {
      result.push_back(t.trackId());
    }
  }
  return result;
}


void
MuonHitMatcher::matchCSCSimHitsToSimTrack(std::vector<unsigned int> track_ids, const edm::PSimHitContainer& csc_hits)
{
  for (const auto& track_id: track_ids)
  {
    for (const auto& h: csc_hits)
    {
      if (h.trackId() != track_id) continue;
      int pdgid = h.particleType();
      if (simMuOnlyCSC_ && std::abs(pdgid) != 13) continue;
      // discard electron hits in the CSC chambers
      if (discardEleHitsCSC_ && pdgid == 11) continue;
      const LocalPoint& lp = h.entryPoint();
      GlobalPoint gp;
      if (MuonHitHelper::isCSC(h.detUnitId()))
       {
     	 gp = cscGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
	 if (verboseCSC_) std::cout <<" csc id "<< CSCDetId(h.detUnitId()) <<" x "<< gp.x()<<" y "<< gp.y() <<" z "<< gp.z()<< std::endl;
       }

      csc_detid_to_hits_[ h.detUnitId() ].push_back(h);
      csc_hits_.push_back(h);
      const CSCDetId& layer_id( h.detUnitId() );
      csc_chamber_to_hits_[ layer_id.chamberId().rawId() ].push_back(h);
    }
  }
}


void
MuonHitMatcher::matchRPCSimHitsToSimTrack(std::vector<unsigned int> track_ids, const edm::PSimHitContainer& rpc_hits)
{
  for (const auto& track_id: track_ids)
  {
    for (const auto& h: rpc_hits)
    {
      if (h.trackId() != track_id) continue;
      int pdgid = h.particleType();
      if (simMuOnlyRPC_ && std::abs(pdgid) != 13) continue;
      // discard electron hits in the RPC chambers
      if (discardEleHitsRPC_ && pdgid == 11) continue;
      rpc_detid_to_hits_[ h.detUnitId() ].push_back(h);
      rpc_hits_.push_back(h);
      const RPCDetId& layer_id( h.detUnitId() );
      rpc_chamber_to_hits_[ layer_id.chamberId().rawId() ].push_back(h);
    }
  }
}


void
MuonHitMatcher::matchGEMSimHitsToSimTrack(std::vector<unsigned int> track_ids, const edm::PSimHitContainer& gem_hits)
{
  for (const auto& track_id: track_ids)
  {
    for (const auto& h: gem_hits)
    {
      if (h.trackId() != track_id) continue;
      int pdgid = h.particleType();
      if (simMuOnlyGEM_ && std::abs(pdgid) != 13) continue;
      // discard electron hits in the GEM chambers
      if (discardEleHitsGEM_ && pdgid == 11) continue;

      const GEMDetId& p_id( h.detUnitId() );

      gem_detid_to_hits_[ h.detUnitId() ].push_back(h);
      gem_hits_.push_back(h);
      gem_chamber_to_hits_[ p_id.chamberId().rawId() ].push_back(h);
      GEMDetId superch_id(p_id.region(), p_id.ring(), p_id.station(), 0, p_id.chamber(), 0);
      gem_superchamber_to_hits_[ superch_id() ].push_back(h);
    }
  }

  // find pads with hits
  const auto& detids = detIdsGEM();
  // find 2-layer coincidence pads with hits
  for (const auto& d: detids) {
    GEMDetId id(d);
    const auto& hits = hitsInDetId(d);
    const auto& roll = gemGeometry_->etaPartition(id);
    //int max_npads = roll->npads();
    set<int> pads;
    for (const auto& h: hits) {
      const LocalPoint& lp = h.entryPoint();
      pads.insert( 1 + static_cast<int>(roll->padTopology().channel(lp)) );
    }
    gem_detids_to_pads_[d] = pads;
  }

  // find 2-layer coincidence pads with hits
  for (const auto& d: detids) {
    GEMDetId id1(d);
    if (id1.layer() != 1) continue;

    // find pads with hits in layer1
    const auto& hits1 = hitsInDetId(d);
    const auto& roll1 = gemGeometry_->etaPartition(id1);
    set<int> pads1;
    set<int> pads2;
    set<int> copads;

    for (const auto& h: hits1) {
      const LocalPoint& lp = h.entryPoint();
      pads1.insert( 1 + static_cast<int>(roll1->padTopology().channel(lp)) );
      if (verboseGEM_) std::cout <<"GEMHits detid1 "<<id1 <<" pad1 "<< 1 + static_cast<int>(roll1->padTopology().channel(lp)) << std::endl;
    }

    // find pads with hits in layer2
    for (const auto& d2 : detids){
      //staggered geometry???? improve here !!
      GEMDetId id2(d2);
      // does layer 2 has simhits?
      if (id2.layer() !=2 or id2.region() != id1.region() or id2.ring()!=id1.ring() or id2.station()!=id1.station() or abs(id2.roll()-id1.roll())>1)
        continue;
      const auto& hits2 = hitsInDetId(id2());
      const auto& roll2 = gemGeometry_->etaPartition(id2);
      for (const auto& h: hits2) {
        const LocalPoint& lp = h.entryPoint();
        pads2.insert( 1 + static_cast<int>(roll2->padTopology().channel(lp)) );
        if (verboseGEM_) std::cout <<"GEMHits detid2 "<<id2 <<" pad2 "<< 1 + static_cast<int>(roll2->padTopology().channel(lp)) << std::endl;
      }
    }

    for (const auto& pad1 : pads1)
      for (const auto& pad2 : pads2){
        if (abs(pad1-pad2) <= 1)
          {
            if (copads.find(pad1) == copads.end())
              copads.insert(pad1);
            if (copads.find(pad2) == copads.end())
              copads.insert(pad2);
          }
      }

    //std::set_intersection(pads1.begin(), pads1.end(), pads2.begin(), pads2.end(), std::inserter(copads, copads.begin()));
    if (copads.empty()) continue;
    gem_detids_to_copads_[d] = copads;//detids here is layer1 id
  }
}


void
MuonHitMatcher::matchME0SimHitsToSimTrack(std::vector<unsigned int> track_ids, const edm::PSimHitContainer& me0_hits)
{
  for (const auto& track_id: track_ids)
  {
    for (const auto& h: me0_hits)
    {
      if (h.trackId() != track_id) continue;
      if (verboseME0_) cout <<"simhit in ME0 detid "<< ME0DetId(h.detUnitId()) <<" id "<< h.particleType() <<" position x "<< h.entryPoint().x()<<" y "<< h.entryPoint().y()<< endl;
      int pdgid = h.particleType();
      //if (simMuOnlyME0_ && std::abs(pdgid) != 13) continue;
      // discard electron hits in the ME0 chambers
      if (discardEleHitsME0_ && std::abs(pdgid) == 11) continue;

      me0_detid_to_hits_[ h.detUnitId() ].push_back(h);
      me0_hits_.push_back(h);
      const ME0DetId& layer_id( h.detUnitId() );
      me0_chamber_to_hits_[ layer_id.layerId().rawId() ].push_back(h);
      me0_superchamber_to_hits_[ layer_id.chamberId().rawId() ].push_back(h);
    }
  }
}


void
MuonHitMatcher::matchDTSimHitsToSimTrack(std::vector<unsigned int> track_ids, const edm::PSimHitContainer& dt_hits)
{
  for (const auto& track_id: track_ids)
  {
    for (const auto& h: dt_hits)
    {
      if (h.trackId() != track_id) continue;
      int pdgid = h.particleType();
      if (simMuOnlyDT_ && std::abs(pdgid) != 13) continue;
      // discard electron hits in the DT chambers
      if (discardEleHitsDT_ && pdgid == 11) continue;

      dt_detid_to_hits_[ h.detUnitId() ].push_back(h);
      const DTWireId layer_id( h.detUnitId() );
      dt_hits_.push_back(h);
      dt_layer_to_hits_ [ layer_id.layerId().rawId() ].push_back(h);
      dt_superlayer_to_hits_ [ layer_id.superlayerId().rawId() ].push_back(h);
      dt_chamber_to_hits_[ layer_id.chamberId().rawId() ].push_back(h);
    }
  }
}


const edm::PSimHitContainer&
MuonHitMatcher::simHits(int sub) const
{
  switch(sub) {
  case MuonSubdetId::GEM:
    return gem_hits_;
  case MuonSubdetId::CSC:
    return csc_hits_;
  case MuonSubdetId::ME0:
    return me0_hits_;
  case MuonSubdetId::RPC:
    return rpc_hits_;
  case MuonSubdetId::DT:
    return dt_hits_;
  }
  return no_hits_;
}


std::set<unsigned int>
MuonHitMatcher::detIdsGEM(int gem_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: gem_detid_to_hits_)
  {
    const auto& id = p.first;
    if (gem_type > 0)
    {
      GEMDetId detId(id);
      if (MuonHitHelper::toGEMType(detId.station(), detId.ring()) != gem_type) continue;
    }
    result.insert(id);
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::detIdsRPC(int rpc_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: rpc_detid_to_hits_)
  {
    const auto& id = p.first;
    if (rpc_type > 0)
    {
      RPCDetId detId(id);
      if (MuonHitHelper::toRPCType(detId.region(), detId.station(), detId.ring()) != rpc_type) continue;
    }
    result.insert(id);
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::detIdsME0() const
{
  std::set<unsigned int> result;
  for (const auto& p: me0_detid_to_hits_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
MuonHitMatcher::detIdsCSC(int csc_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: csc_detid_to_hits_)
  {
    const auto& id = p.first;
    if (csc_type > 0)
    {
      CSCDetId detId(id);
      if (MuonHitHelper::toCSCType(detId.station(), detId.ring()) != csc_type) continue;
    }
    result.insert(id);
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::detIdsDT(int dt_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: dt_detid_to_hits_)
  {
    const auto& id = p.first;
    if (dt_type > 0)
    {
      DTWireId detId(id);
      if (MuonHitHelper::toDTType(detId.wheel(), detId.station()) != dt_type) continue;
    }
    result.insert(id);
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::detIdsGEMCoincidences() const
{
  std::set<unsigned int> result;
  for (const auto& p: gem_detids_to_copads_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
MuonHitMatcher::detIdsME0Coincidences(int min_n_layers) const
{
  std::set<unsigned int> result;

  //   int result = 0;
  //   const auto& chamber_ids = chamberIdsCSC();
  //   for (const auto& id: chamber_ids)
  //   {
  //     if (nLayersWithHitsInSuperChamber(id) >= min_n_layers) result += 1;
  //   }
  //   return result;

  //   for (const auto& p: me0_detids_to_copads_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
MuonHitMatcher::chamberIdsGEM(int gem_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: gem_chamber_to_hits_)
  {
    const auto& id = p.first;
    if (gem_type > 0)
    {
      GEMDetId detId(id);
      if (MuonHitHelper::toGEMType(detId.station(), detId.ring()) != gem_type) continue;
    }
    result.insert(id);
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::chamberIdsRPC(int rpc_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: rpc_chamber_to_hits_)
  {
    const auto& id = p.first;
    if (rpc_type > 0)
    {
      RPCDetId detId(id);
      if (MuonHitHelper::toRPCType(detId.region(), detId.station(), detId.ring()) != rpc_type) continue;
    }
    result.insert(id);
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::chamberIdsME0() const
{
  std::set<unsigned int> result;
  for (const auto& p: me0_chamber_to_hits_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
MuonHitMatcher::chamberIdsCSC(int csc_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: csc_chamber_to_hits_)
  {
    const auto& id = p.first;
    if (csc_type > 0)
    {
      CSCDetId detId(id);
      if (MuonHitHelper::toCSCType(detId.station(), detId.ring()) != csc_type) continue;
    }
    result.insert(id);
  }
  return result;
}

std::set<unsigned int>
MuonHitMatcher::chamberIdsDT(int dt_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: dt_chamber_to_hits_)
  {
    const auto& id = p.first;
    if (dt_type > 0)
    {
      DTChamberId detId(id);
      if (MuonHitHelper::toDTType(detId.wheel(), detId.station()) != dt_type) continue;
    }
    result.insert(id);
  }
  return result;
}

std::set<unsigned int>
MuonHitMatcher::superChamberIdsGEM() const
{
  std::set<unsigned int> result;
  for (const auto& p: gem_superchamber_to_hits_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
MuonHitMatcher::superChamberIdsME0() const
{
  std::set<unsigned int> result;
  for (const auto& p: me0_superchamber_to_hits_) result.insert(p.first);
  return result;
}


std::set<unsigned int>
MuonHitMatcher::superChamberIdsGEMCoincidences() const
{
  std::set<unsigned int> result;
  for (const auto& p: gem_detids_to_copads_)
  {
    const GEMDetId& p_id(p.first);
    GEMDetId superch_id(p_id.region(), p_id.ring(), p_id.station(), 0, p_id.chamber(), 0);
    //std::cout <<"superChamberIdsGEMCoincidences id "<< superch_id << std::endl;
    result.insert(superch_id.rawId());
  }
  return result;
}

std::set<unsigned int>
MuonHitMatcher::layerIdsDT() const
{
  std::set<unsigned int> result;
  for (const auto& p: dt_layer_to_hits_) result.insert(p.first);
  return result;
}

std::set<unsigned int>
MuonHitMatcher::superlayerIdsDT() const
{
  std::set<unsigned int> result;
  for (const auto& p: dt_superlayer_to_hits_) result.insert(p.first);
  return result;
}

const edm::PSimHitContainer&
MuonHitMatcher::hitsInDetId(unsigned int detid) const
{
  if (MuonHitHelper::isGEM(detid))
  {
    if (gem_detid_to_hits_.find(detid) == gem_detid_to_hits_.end()) return no_hits_;
    return gem_detid_to_hits_.at(detid);
  }
  if (MuonHitHelper::isME0(detid))
  {
    if (me0_detid_to_hits_.find(detid) == me0_detid_to_hits_.end()) return no_hits_;
    return me0_detid_to_hits_.at(detid);
  }
  if (MuonHitHelper::isCSC(detid))
  {
    if (csc_detid_to_hits_.find(detid) == csc_detid_to_hits_.end()) return no_hits_;
    return csc_detid_to_hits_.at(detid);
  }
  if (MuonHitHelper::isRPC(detid))
  {
    if (rpc_detid_to_hits_.find(detid) == rpc_detid_to_hits_.end()) return no_hits_;
    return rpc_detid_to_hits_.at(detid);
  }
  if(MuonHitHelper::isDT(detid))
  {
    if (dt_detid_to_hits_.find(detid) == dt_detid_to_hits_.end()) return no_hits_;
    return dt_detid_to_hits_.at(detid);
  }
  return no_hits_;
}


const edm::PSimHitContainer&
MuonHitMatcher::hitsInChamber(unsigned int detid) const
{
  if (MuonHitHelper::isGEM(detid)) // make sure we use chamber id
  {
    const GEMDetId id(detid);
    if (gem_chamber_to_hits_.find(id.chamberId().rawId()) == gem_chamber_to_hits_.end()) return no_hits_;
    return gem_chamber_to_hits_.at(id.chamberId().rawId());
  }
  if (MuonHitHelper::isME0(detid)) // make sure we use chamber id
  {
    const ME0DetId id(detid);
    if (me0_chamber_to_hits_.find(id.layerId().rawId()) == me0_chamber_to_hits_.end()) return no_hits_;
    return me0_chamber_to_hits_.at(id.layerId().rawId());
  }
  if (MuonHitHelper::isCSC(detid))
  {
    const CSCDetId id(detid);
    if (csc_chamber_to_hits_.find(id.chamberId().rawId()) == csc_chamber_to_hits_.end()) return no_hits_;
    return csc_chamber_to_hits_.at(id.chamberId().rawId());
  }
  if (MuonHitHelper::isRPC(detid))
  {
    const RPCDetId id(detid);
    if (rpc_chamber_to_hits_.find(id.chamberId().rawId()) == rpc_chamber_to_hits_.end()) return no_hits_;
    return rpc_chamber_to_hits_.at(id.chamberId().rawId());
  }
  if(MuonHitHelper::isDT(detid))
  {
    const DTWireId id(detid);
    if(dt_chamber_to_hits_.find(id.chamberId().rawId()) == dt_chamber_to_hits_.end()) return no_hits_;
    return dt_chamber_to_hits_.at(id.chamberId().rawId());
  }
  return no_hits_;
}


const edm::PSimHitContainer&
MuonHitMatcher::hitsInSuperChamber(unsigned int detid) const
{
  if (MuonHitHelper::isGEM(detid))
  {
    const GEMDetId id(detid);
    if (gem_superchamber_to_hits_.find(id.chamberId().rawId()) == gem_superchamber_to_hits_.end()) return no_hits_;
    return gem_superchamber_to_hits_.at(id.chamberId().rawId());
  }
  if (MuonHitHelper::isCSC(detid)) return hitsInChamber(detid);
  if (MuonHitHelper::isME0(detid))
  {
    const ME0DetId id(detid);
    if (me0_superchamber_to_hits_.find(id.chamberId().rawId()) == me0_superchamber_to_hits_.end()) return no_hits_;
    return me0_superchamber_to_hits_.at(id.chamberId().rawId());
  }

  return no_hits_;
}

const edm::PSimHitContainer&
MuonHitMatcher::hitsInLayerDT(unsigned int detid) const
{
  if (!MuonHitHelper::isDT(detid)) return no_hits_;

  const DTWireId id(detid);
  if (dt_layer_to_hits_.find(id.layerId().rawId()) == dt_layer_to_hits_.end()) return no_hits_;
  return dt_layer_to_hits_.at(id.layerId().rawId());
}

const edm::PSimHitContainer&
MuonHitMatcher::hitsInSuperLayerDT(unsigned int detid) const
{
  if (!MuonHitHelper::isDT(detid)) return no_hits_;

  const DTWireId id(detid);
  if (dt_superlayer_to_hits_.find(id.superlayerId().rawId()) == dt_superlayer_to_hits_.end()) return no_hits_;
  return dt_superlayer_to_hits_.at(id.superlayerId().rawId());
}

const edm::PSimHitContainer&
MuonHitMatcher::hitsInChamberDT(unsigned int detid) const
{
  if (!MuonHitHelper::isDT(detid)) return no_hits_;

  const DTWireId id(detid);
  if (dt_chamber_to_hits_.find(id.chamberId().rawId()) == dt_chamber_to_hits_.end()) return no_hits_;
  return dt_chamber_to_hits_.at(id.chamberId().rawId());
}

int
MuonHitMatcher::nLayersWithHitsInSuperChamber(unsigned int detid) const
{
  set<int> layers_with_hits;
  const auto& hits = hitsInSuperChamber(detid);
  for (const auto& h: hits)
  {
    if (MuonHitHelper::isGEM(detid))
    {
      const GEMDetId& idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
    if (MuonHitHelper::isME0(detid))
    {
      const ME0DetId& idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
    if (MuonHitHelper::isCSC(detid))
    {
      const CSCDetId& idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
    if (MuonHitHelper::isRPC(detid))
    {
      const RPCDetId& idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
  }
  return layers_with_hits.size();
}


bool
MuonHitMatcher::hitStationGEM(int st, int nlayers) const
{
  int nst=0;
  for(const auto& ddt: chamberIdsGEM()) {

    const GEMDetId id(ddt);
    if (id.station()!=st) continue;

    const int nl(nLayersWithHitsInSuperChamber(id.rawId()));
    if (nl < nlayers) continue;
    ++nst;
  }
  return nst;
}


bool
MuonHitMatcher::hitStationRPC(int st) const
{
  int nst=0;
  for(const auto& ddt: chamberIdsRPC()) {
    const RPCDetId id(ddt);
    if (id.station()!=st) continue;
    ++nst;
  }
  return nst;
}


bool
MuonHitMatcher::hitStationCSC(int st, int nlayers) const
{
  int nst=0;
  for(const auto& ddt: chamberIdsCSC()) {

    const CSCDetId id(ddt);
    int ri(id.ring());
    if (id.station()!=st) continue;

    // ME1/a case - check the ME1/b chamber
    if (st==1 and ri==4) {
      CSCDetId idME1b(id.endcap(), id.station(), 1, id.chamber(), 0);
      const int nl1a(nLayersWithHitsInSuperChamber(id.rawId()));
      const int nl1b(nLayersWithHitsInSuperChamber(idME1b.rawId()));
      if (nl1a + nl1b < nlayers) continue;
      ++nst;
    }
    // ME1/b case - check the ME1/a chamber
    else if (st==1 and ri==1) {
      CSCDetId idME1a(id.endcap(), id.station(), 4, id.chamber(), 0);
      const int nl1a(nLayersWithHitsInSuperChamber(idME1a.rawId()));
      const int nl1b(nLayersWithHitsInSuperChamber(id.rawId()));
      if (nl1a + nl1b < nlayers) continue;
      ++nst;
    }
    // default case
    else {
      const int nl(nLayersWithHitsInSuperChamber(id.rawId()));
      if (nl < nlayers) continue;
      ++nst;
    }
  }
  return nst;
}

bool
MuonHitMatcher::hitStationDT(int st, int nsuperlayers, int nlayers) const
{
  int nst=0;
  for(const auto& ddt: chamberIdsDT()) {

    const DTChamberId id(ddt);
    if (id.station()!=st) continue;

    // require at least 1 superlayer
    const int nsl(nSuperLayersWithHitsInChamberDT(id.rawId()));
    if (nsl < nsuperlayers) continue;

    // require at least 3 layers hit per chamber
    const int nl(nLayersWithHitsInChamberDT(id.rawId()));
    if (nl < nlayers) continue;
    ++nst;
  }
  return nst;
}

int
MuonHitMatcher::nStationsCSC(int nlayers) const
{
  return (hitStationCSC(1, nlayers) + hitStationCSC(2, nlayers) +
	  hitStationCSC(3, nlayers) + hitStationCSC(4, nlayers));
}

int
MuonHitMatcher::nStationsDT(int nsuperlayers, int nlayers) const
{
  return (hitStationDT(1, nsuperlayers, nlayers) + hitStationDT(2, nsuperlayers, nlayers) +
	  hitStationDT(3, nsuperlayers, nlayers) + hitStationDT(4,nsuperlayers, nlayers));
}


int
MuonHitMatcher::nStationsRPC() const
{
  return (hitStationRPC(1) + hitStationRPC(2) + hitStationRPC(3) + hitStationRPC(4));
}


int
MuonHitMatcher::nStationsGEM(int nlayers) const
{
  return (hitStationGEM(1, nlayers) + hitStationGEM(3, nlayers));
}

int
MuonHitMatcher::nCellsWithHitsInLayerDT(unsigned int detid) const
{
  set<int> layers_with_hits;
  const auto& hits = hitsInLayerDT(detid);
  for (const auto& h: hits) {
    if (MuonHitHelper::isDT(detid)) {
      const DTWireId idd(h.detUnitId());
      layers_with_hits.insert(idd.wire());
    }
  }
  return layers_with_hits.size();
}

int
MuonHitMatcher::nLayersWithHitsInSuperLayerDT(unsigned int detid) const
{
  set<int> layers_with_hits;
  const auto& hits = hitsInSuperLayerDT(detid);
  for (const auto& h: hits) {
    if (MuonHitHelper::isDT(detid)) {
      const DTLayerId idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
  }
  return layers_with_hits.size();
}

int
MuonHitMatcher::nSuperLayersWithHitsInChamberDT(unsigned int detid) const
{
  set<int> sl_with_hits;
  const auto& hits = hitsInChamber(detid);
  for (const auto& h: hits) {
    if (MuonHitHelper::isDT(detid)) {
      const DTSuperLayerId idd(h.detUnitId());
      sl_with_hits.insert(idd.superLayer());
    }
  }
  return sl_with_hits.size();
}

int
MuonHitMatcher::nLayersWithHitsInChamberDT(unsigned int detid) const
{
  int nLayers = 0;
  const auto& superLayers(dtGeometry_->chamber(DTChamberId(detid))->superLayers());
  for (const auto& sl: superLayers) {
    nLayers += nLayersWithHitsInSuperLayerDT(sl->id().rawId());
  }
  return nLayers;
}

GlobalPoint
MuonHitMatcher::simHitsMeanPosition(const edm::PSimHitContainer& sim_hits) const
{
  if (sim_hits.empty()) return GlobalPoint(); // point "zero"

  float sumx, sumy, sumz;
  sumx = sumy = sumz = 0.f;
  size_t n = 0;
  for (const auto& h: sim_hits)
  {
    const LocalPoint& lp = h.entryPoint();
    GlobalPoint gp;
    if ( MuonHitHelper::isGEM(h.detUnitId()) )
    {
      gp = gemGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if ( MuonHitHelper::isME0(h.detUnitId()) )
    {
      gp = me0Geometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if (MuonHitHelper::isCSC(h.detUnitId()))
    {
      gp = cscGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if (MuonHitHelper::isRPC(h.detUnitId()))
    {
      gp = rpcGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if (MuonHitHelper::isDT(h.detUnitId()))
    {
      gp = dtGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else continue;
    sumx += gp.x();
    sumy += gp.y();
    sumz += gp.z();
    ++n;
  }
  if (n == 0) return GlobalPoint();
  return GlobalPoint(sumx/n, sumy/n, sumz/n);
}

GlobalVector
MuonHitMatcher::simHitsMeanMomentum(const edm::PSimHitContainer& sim_hits) const
{
  if (sim_hits.empty()) return GlobalVector(); // point "zero"

  float sumx, sumy, sumz;
  sumx = sumy = sumz = 0.f;
  size_t n = 0;
  for (const auto& h: sim_hits)
  {
    const LocalVector& lv = h.momentumAtEntry();
    GlobalVector gv;
    if ( MuonHitHelper::isGEM(h.detUnitId()) )
    {
      gv = gemGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if ( MuonHitHelper::isME0(h.detUnitId()) )
    {
      gv = me0Geometry_->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if (MuonHitHelper::isCSC(h.detUnitId()))
    {
      gv = cscGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if (MuonHitHelper::isRPC(h.detUnitId()))
    {
      gv = rpcGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if (MuonHitHelper::isDT(h.detUnitId()))
    {
      gv = dtGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else continue;
    sumx += gv.x();
    sumy += gv.y();
    sumz += gv.z();
    ++n;
  }
  if (n == 0) return GlobalVector();
  return GlobalVector(sumx/n, sumy/n, sumz/n);
}


float
MuonHitMatcher::simHitsGEMCentralPosition(const edm::PSimHitContainer& sim_hits) const
{
  if (sim_hits.empty()) return -0.0; // point "zero"

  float central = -0.0;
  size_t n = 0;
  for (const auto& h: sim_hits)
  {
    LocalPoint lp( 0., 0., 0. );//local central
    GlobalPoint gp;
    if ( MuonHitHelper::isGEM(h.detUnitId()) )
    {
      gp = gemGeometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if ( MuonHitHelper::isME0(h.detUnitId()) )
    {
      gp = me0Geometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else continue;
    central = gp.perp();
    //std::cout <<" GEMid "<< GEMDetId(h.detUnitId()) <<"cenral gp perp "<< gp.perp()<<" x "<< gp.x()<<" y "<< gp.y() <<" z "<< gp.z() <<std::endl;
    if (n>=1) std::cout <<"warning! find more than one simhits in GEM chamber " << std::endl;
    ++n;
  }

  return central;
}




float
MuonHitMatcher::LocalBendingInChamber(unsigned int detid) const
{
  const CSCDetId cscid(detid);
  if (nLayersWithHitsInSuperChamber(detid)<6) return -100;
  float phi_layer1=-10;
  float phi_layer6=10;

  if (cscid.station()==1 and (cscid.ring()==1 or cscid.ring()==4)){
  	const CSCDetId cscid1a(cscid.endcap(), cscid.station(), 4, cscid.chamber(), 1);
  	const CSCDetId cscid1b(cscid.endcap(), cscid.station(), 1, cscid.chamber(), 1);
    const edm::PSimHitContainer& hits1a = hitsInDetId(cscid1a.rawId());
    const edm::PSimHitContainer& hits1b = hitsInDetId(cscid1b.rawId());
    const GlobalPoint& gp1a = simHitsMeanPosition(hitsInDetId(cscid1a.rawId()));
    const GlobalPoint& gp1b = simHitsMeanPosition(hitsInDetId(cscid1b.rawId()));
    if (hits1a.size()>0 and hits1b.size()>0)
	    //phi_layer1 = (gp1a.phi()*hits1a.size()+gp1b.phi()*hits1b.size())/(hits1a.size()+hits1b.size());
	    phi_layer1 = (gp1a.phi()+gp1b.phi())/2.0;
    else if (hits1a.size()>0) phi_layer1 = gp1a.phi();
    else if (hits1b.size()>0) phi_layer1 = gp1b.phi();
    else std::cerr <<" no hits in layer1, cant not find global phi of hits " << std::endl;

  	const CSCDetId cscid6a(cscid.endcap(), cscid.station(), 4, cscid.chamber(), 6);
  	const CSCDetId cscid6b(cscid.endcap(), cscid.station(), 1, cscid.chamber(), 6);
    const edm::PSimHitContainer& hits6a = hitsInDetId(cscid6a.rawId());
    const edm::PSimHitContainer& hits6b = hitsInDetId(cscid6b.rawId());
    const GlobalPoint& gp6a = simHitsMeanPosition(hitsInDetId(cscid6a.rawId()));
    const GlobalPoint& gp6b = simHitsMeanPosition(hitsInDetId(cscid6b.rawId()));
    if (hits6a.size()>0 and hits6b.size()>0)
	    //phi_layer6 = (gp6a.phi()*hits6a.size()+gp6b.phi()*hits6b.size())/(hits6a.size()+hits6b.size());
	    phi_layer6 = (gp6a.phi()+gp6b.phi())/2.0;
    else if (hits6a.size()>0) phi_layer6 = gp6a.phi();
    else if (hits6b.size()>0) phi_layer6 = gp6b.phi();
    else std::cerr <<" no hits in layer6, cant not find global phi of hits " << std::endl;


  }
  else {
  	const CSCDetId cscid1(cscid.endcap(), cscid.station(), cscid.ring(), cscid.chamber(), 1);
    const edm::PSimHitContainer& hits1 = hitsInDetId(cscid1.rawId());
    if (hits1.size()==0) std::cerr <<" no hits in layer1, cant not find global phi of hits " << std::endl;
    const GlobalPoint& gp1 = simHitsMeanPosition(hitsInDetId(cscid1.rawId()));
    phi_layer1 = gp1.phi();

  	const CSCDetId cscid6(cscid.endcap(), cscid.station(), cscid.ring(), cscid.chamber(), 6);
    const edm::PSimHitContainer& hits6 = hitsInDetId(cscid6.rawId());
    if (hits6.size()==0) std::cerr <<" no hits in layer6, cant not find global phi of hits " << std::endl;
    const GlobalPoint& gp6 = simHitsMeanPosition(hitsInDetId(cscid6.rawId()));
    phi_layer6 = gp6.phi();
  }
	//std::cout <<" phi1 "<< phi_layer1 <<" phi6 " << phi_layer6 << std::endl;
	return deltaPhi(phi_layer6,phi_layer1);
}


float
MuonHitMatcher::simHitsMeanStrip(const edm::PSimHitContainer& sim_hits) const
{
  if (sim_hits.empty()) return -1.f;

  float sums = 0.f;
  size_t n = 0;
  for (const auto& h: sim_hits)
  {
    const LocalPoint& lp = h.entryPoint();
    float s;
    const auto& d = h.detUnitId();
    if ( MuonHitHelper::isGEM(d) )
    {
      s = gemGeometry_->etaPartition(d)->strip(lp);
    }
    else if ( MuonHitHelper::isME0(d) )
    {
      s = me0Geometry_->etaPartition(d)->strip(lp);
    }
    else if (MuonHitHelper::isCSC(d))
    {
      s = cscGeometry_->layer(d)->geometry()->strip(lp);
      // convert to half-strip:
      s *= 2.;
    }
    else if (MuonHitHelper::isRPC(d))
    {
      s = rpcGeometry_->roll(d)->strip(lp);
    }
    else continue;
    sums += s;
    ++n;
  }
  if (n == 0) return -1.f;
  return sums/n;
}


float
MuonHitMatcher::simHitsMeanWG(const edm::PSimHitContainer& sim_hits) const
{
  if (sim_hits.empty()) return -1.f;

  float sums = 0.f;
  size_t n = 0;
  for (const auto& h: sim_hits)
  {
    const LocalPoint& lp = h.entryPoint();
    float s;
    const auto& d = h.detUnitId();
    if (MuonHitHelper::isCSC(d))
    {
      // find nearest wire
      int nearestWire(cscGeometry_->layer(d)->geometry()->nearestWire(lp));
      // then find the corresponding wire group
      s = cscGeometry_->layer(d)->geometry()->wireGroup(nearestWire);
    }
    else continue;
    sums += s;
    ++n;
  }
  if (n == 0) return -1.f;
  return sums/n;
}


float
MuonHitMatcher::simHitsMeanWire(const edm::PSimHitContainer& sim_hits) const
{
  if (sim_hits.empty()) return -1.f;

  float sums = 0.f;
  size_t n = 0;
  for (const auto& h: sim_hits)
  {
    const LocalPoint& lp = h.entryPoint();
    float s;
    const auto& d = h.detUnitId();
    if (MuonHitHelper::isDT(d))
    {
      // find nearest wire
      s  = dtGeometry_->layer(DTLayerId(d))->specificTopology().channel(lp);
    }
    else continue;
    sums += s;
    ++n;
  }
  if (n == 0) return -1.f;
  return sums/n;
}


std::set<int>
MuonHitMatcher::hitStripsInDetId(unsigned int detid, int margin_n_strips) const
{
  set<int> result;
  const auto& simhits = hitsInDetId(detid);
  if ( MuonHitHelper::isGEM(detid) )
  {
    GEMDetId id(detid);
    int max_nstrips = gemGeometry_->etaPartition(id)->nstrips();
    for (const auto& h: simhits)
    {
      const LocalPoint& lp = h.entryPoint();
      int central_strip = static_cast<int>(gemGeometry_->etaPartition(id)->topology().channel(lp));
      int smin = central_strip - margin_n_strips;
      smin = (smin > 0) ? smin : 1;
      int smax = central_strip + margin_n_strips;
      smax = (smax <= max_nstrips) ? smax : max_nstrips;
      for (int ss = smin; ss <= smax; ++ss) result.insert(ss);
    }
  }
  else if ( MuonHitHelper::isME0(detid) )
  {
    ME0DetId id(detid);
    int max_nstrips = me0Geometry_->etaPartition(id)->nstrips();
    for (const auto& h: simhits)
    {
      const LocalPoint& lp = h.entryPoint();
      int central_strip = 1 + static_cast<int>(me0Geometry_->etaPartition(id)->topology().channel(lp));
      int smin = central_strip - margin_n_strips;
      smin = (smin > 0) ? smin : 1;
      int smax = central_strip + margin_n_strips;
      smax = (smax <= max_nstrips) ? smax : max_nstrips;
      for (int ss = smin; ss <= smax; ++ss) result.insert(ss);
    }
  }
  else if ( MuonHitHelper::isCSC(detid) )
  {
    CSCDetId id(detid);
    int max_nstrips = cscGeometry_->layer(id)->geometry()->numberOfStrips();
    for (const auto& h: simhits)
    {
      const LocalPoint& lp = h.entryPoint();
      int central_strip = cscGeometry_->layer(id)->geometry()->nearestStrip(lp);
      int smin = central_strip - margin_n_strips;
      smin = (smin > 0) ? smin : 1;
      int smax = central_strip + margin_n_strips;
      smax = (smax <= max_nstrips) ? smax : max_nstrips;
      for (int ss = smin; ss <= smax; ++ss) result.insert(ss);
    }
  }
  else if ( MuonHitHelper::isRPC(detid) )
  {
    RPCDetId id(detid);
    for (const auto& roll: rpcGeometry_->chamber(id)->rolls()) {
      int max_nstrips = roll->nstrips();
      for (const auto& h: hitsInDetId(roll->id().rawId())) {
        const LocalPoint& lp = h.entryPoint();
        // check how the RPC strip numbers start counting - Ask Piet!!!
        int central_strip = static_cast<int>(roll->topology().channel(lp));
        // int central_strip2 = 1 + static_cast<int>(rpcGeometry_->roll(id)->strip(lp));
        // std::cout <<"strip from topology"<< central_strip <<" strip from roll" << central_strip2 <<std::endl;
        int smin = central_strip - margin_n_strips;
        smin = (smin > 0) ? smin : 1;
        int smax = central_strip + margin_n_strips;
        smax = (smax <= max_nstrips) ? smax : max_nstrips;
        for (int ss = smin; ss <= smax; ++ss) result.insert(ss);
      }
    }
  }
  return result;
}


std::set<int>
MuonHitMatcher::hitWiregroupsInDetId(unsigned int detid, int margin_n_wg) const
{
  set<int> result;
  if ( MuonHitHelper::isCSC(detid) ) {
    const auto& simhits = hitsInDetId(detid);
    CSCDetId id(detid);
    int max_n_wg = cscGeometry_->layer(id)->geometry()->numberOfWireGroups();
    for (const auto& h: simhits) {
      const LocalPoint& lp = h.entryPoint();
      const auto& layer_geo = cscGeometry_->layer(id)->geometry();
      int central_wg = layer_geo->wireGroup(layer_geo->nearestWire(lp));
      int wg_min = central_wg - margin_n_wg;
      wg_min = (wg_min > 0) ? wg_min : 1;
      int wg_max = central_wg + margin_n_wg;
      wg_max = (wg_max <= max_n_wg) ? wg_max : max_n_wg;
      for (int wg = wg_min; wg <= wg_max; ++wg) result.insert(wg);
    }
  }
  return result;
}


std::set<int>
MuonHitMatcher::hitPadsInDetId(unsigned int detid) const
{
  set<int> none;
  if (gem_detids_to_pads_.find(detid) == gem_detids_to_pads_.end()) return none;
  return gem_detids_to_pads_.at(detid);
}


std::set<int>
MuonHitMatcher::hitCoPadsInDetId(unsigned int detid) const
{
  set<int> none;
  if (gem_detids_to_copads_.find(detid) == gem_detids_to_copads_.end()) return none;
  return gem_detids_to_copads_.at(detid);
}


std::set<unsigned int>
MuonHitMatcher::hitWiresInDTLayerId(unsigned int detid, int margin_n_wires) const
{
  set<unsigned int> result;
  if ( MuonHitHelper::isDT(detid) ) {
    DTLayerId id(detid);
    int max_nwires = dtGeometry_->layer(id)->specificTopology().channels();
    for (int wn = 0; wn <= max_nwires; ++wn) {
      DTWireId wid(id,wn);
      for (const auto& h: hitsInDetId(wid.rawId())) {
        if (verboseDT_) cout << "central DTWireId "<< wid << " simhit " <<h<< endl;
        int smin = wn - margin_n_wires;
        smin = (smin > 0) ? smin : 1;
        int smax = wn + margin_n_wires;
        smax = (smax <= max_nwires) ? smax : max_nwires;
        for (int ss = smin; ss <= smax; ++ss) {
          DTWireId widd(id, ss);
          if (verboseDT_) cout << "\tadding DTWireId to collection "<< widd << endl;
          result.insert(widd.rawId());
        }
      }
    }
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::hitWiresInDTSuperLayerId(unsigned int detid, int margin_n_wires) const
{
  set<unsigned int> result;
  const auto& layers(dtGeometry_->superLayer(DTSuperLayerId(detid))->layers());
  for (const auto& l: layers) {
    if (verboseDT_)cout << "hitWiresInDTSuperLayerId::l id "<<l->id() << endl;
    const auto& p(hitWiresInDTLayerId(l->id().rawId(), margin_n_wires));
    result.insert(p.begin(),p.end());
  }
  return result;
}


std::set<unsigned int>
MuonHitMatcher::hitWiresInDTChamberId(unsigned int detid, int margin_n_wires) const
{
  set<unsigned int> result;
  const auto& superLayers(dtGeometry_->chamber(DTChamberId(detid))->superLayers());
  for (const auto& sl: superLayers) {
    if (verboseDT_)cout << "hitWiresInDTChamberId::sl id "<<sl->id() << endl;
    const auto& p(hitWiresInDTSuperLayerId(sl->id().rawId(), margin_n_wires));
    result.insert(p.begin(),p.end());
  }
  return result;
}


std::set<int>
MuonHitMatcher::hitPartitions() const
{
  std::set<int> result;

  const auto& detids = detIdsGEM();
  for (const auto& id: detids)
  {
    GEMDetId idd(id);
    result.insert( idd.roll() );
  }
  return result;
}


int
MuonHitMatcher::nPadsWithHits() const
{
  int result = 0;
  const auto& pad_ids = detIdsGEM();
  for (const auto& id: pad_ids)
  {
    result += hitPadsInDetId(id).size();
  }
  return result;
}


int
MuonHitMatcher::nCoincidencePadsWithHits() const
{
  int result = 0;
  const auto& copad_ids = detIdsGEMCoincidences();
  for (const auto& id: copad_ids)
  {
    result += hitCoPadsInDetId(id).size();
  }
  return result;
}


int
MuonHitMatcher::nCoincidenceCSCChambers(int min_n_layers) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsCSC();
  for (const auto& id: chamber_ids)
  {
    if (nLayersWithHitsInSuperChamber(id) >= min_n_layers) result += 1;
  }
  return result;
}

void
MuonHitMatcher::cscChamberIdsToString(const std::set<unsigned int>& set) const
{
  for (const auto& p: set) {
    CSCDetId detId(p);
    std::cout << " " << detId << "\n";
  }
}


void
MuonHitMatcher::dtChamberIdsToString(const std::set<unsigned int>& set) const
{
  for (const auto& p: set) {
    DTChamberId detId(p);
    std::cout << " " << detId << "\n";
  }
}


std::set<unsigned int>
MuonHitMatcher::chamberIdsCSCStation(int station) const
{
  set<unsigned int> result;
  switch(station){
  case 1: {
    const auto& p1(chamberIdsCSC(MuonHitHelper::CSC_ME1a));
    const auto& p2(chamberIdsCSC(MuonHitHelper::CSC_ME1b));
    const auto& p3(chamberIdsCSC(MuonHitHelper::CSC_ME12));
    const auto& p4(chamberIdsCSC(MuonHitHelper::CSC_ME13));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    break;
  }
  case 2: {
    const auto& p1(chamberIdsCSC(MuonHitHelper::CSC_ME21));
    const auto& p2(chamberIdsCSC(MuonHitHelper::CSC_ME22));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    break;
  }
  case 3: {
    const auto& p1(chamberIdsCSC(MuonHitHelper::CSC_ME31));
    const auto& p2(chamberIdsCSC(MuonHitHelper::CSC_ME32));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    break;
  }
  case 4: {
    const auto& p1(chamberIdsCSC(MuonHitHelper::CSC_ME41));
    const auto& p2(chamberIdsCSC(MuonHitHelper::CSC_ME42));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    break;
  }
  };
  return result;
}


std::set<unsigned int>
MuonHitMatcher::chamberIdsDTStation(int station) const
{
  set<unsigned int> result;
  switch(station){
  case 1: {
    const auto& p1(chamberIdsDT(MuonHitHelper::DT_MB21p));
    const auto& p2(chamberIdsDT(MuonHitHelper::DT_MB11p));
    const auto& p3(chamberIdsDT(MuonHitHelper::DT_MB01));
    const auto& p4(chamberIdsDT(MuonHitHelper::DT_MB11n));
    const auto& p5(chamberIdsDT(MuonHitHelper::DT_MB21n));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    result.insert(p5.begin(),p5.end());
    break;
  }
  case 2: {
    const auto& p1(chamberIdsDT(MuonHitHelper::DT_MB22p));
    const auto& p2(chamberIdsDT(MuonHitHelper::DT_MB12p));
    const auto& p3(chamberIdsDT(MuonHitHelper::DT_MB02));
    const auto& p4(chamberIdsDT(MuonHitHelper::DT_MB12n));
    const auto& p5(chamberIdsDT(MuonHitHelper::DT_MB22n));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    result.insert(p5.begin(),p5.end());
    break;
  }
  case 3: {
    const auto& p1(chamberIdsDT(MuonHitHelper::DT_MB23p));
    const auto& p2(chamberIdsDT(MuonHitHelper::DT_MB13p));
    const auto& p3(chamberIdsDT(MuonHitHelper::DT_MB03));
    const auto& p4(chamberIdsDT(MuonHitHelper::DT_MB13n));
    const auto& p5(chamberIdsDT(MuonHitHelper::DT_MB23n));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    result.insert(p5.begin(),p5.end());
    break;
  }
  case 4: {
    const auto& p1(chamberIdsDT(MuonHitHelper::DT_MB24p));
    const auto& p2(chamberIdsDT(MuonHitHelper::DT_MB14p));
    const auto& p3(chamberIdsDT(MuonHitHelper::DT_MB04));
    const auto& p4(chamberIdsDT(MuonHitHelper::DT_MB14n));
    const auto& p5(chamberIdsDT(MuonHitHelper::DT_MB24n));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    result.insert(p5.begin(),p5.end());
    break;
  }
  };
  return result;
}

GlobalPoint
MuonHitMatcher::simHitsMeanPositionStation(int station) const
{
  // get the mean position in CSC
  edm::PSimHitContainer cscHits;
  const auto& cscIds(chamberIdsCSCStation(station));
  for (const auto& p1 : cscIds){
    const auto& hits = hitsInChamber(p1);
    cscHits.insert(cscHits.end(), hits.begin(), hits.end());
  }
  const GlobalPoint& cscgp = simHitsMeanPosition(cscHits);
  cout << "cscgp eta " << cscgp.eta() << " " << cscgp.phi() << endl;

  edm::PSimHitContainer dtHits;
  const auto& dtIds(chamberIdsDTStation(station));
  for (const auto& p1 : dtIds){
    const auto& hits = hitsInChamber(p1);
    dtHits.insert(dtHits.end(), hits.begin(), hits.end());
  }
  const GlobalPoint& dtgp = simHitsMeanPosition(dtHits);
  cout << "dtgp eta " << dtgp.eta() << " " << dtgp.phi() << endl;

  // get the average
  if (cscgp == GlobalPoint() and dtgp == GlobalPoint())
    return GlobalPoint();

  if (!(cscgp == GlobalPoint()) and dtgp == GlobalPoint())
    return cscgp;

  if (cscgp == GlobalPoint() and !(dtgp == GlobalPoint()))
    return dtgp;

  GlobalPoint average;
  float x, y, z;
  x = (cscgp.x() + dtgp.x())/2.;
  y = (cscgp.y() + dtgp.y())/2.;
  z = (cscgp.z() + dtgp.z())/2.;

  return GlobalPoint(x,y,z);
}
