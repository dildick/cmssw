#include "Validation/MuonHits/interface/MuonHitMatcher.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "TF1.h"
#include "TGraph.h"
#include "TFitResult.h"

#include <algorithm>
#include <iomanip>

using namespace std;


MuonHitMatcher::MuonHitMatcher(const edm::ParameterSet& ps, edm::ConsumesCollector && iC)
{
  event().getByToken(simTrackInput_, sim_tracks);
  event().getByToken(simVertexInput_, sim_vertices);

  const edm::EDGetTokenT<edm::SimVertexContainer>& simVertexInput_;
  const edm::EDGetTokenT<edm::SimTrackContainer>& simTrackInput_;
  const edm::EDGetTokenT<edm::PSimHitContainer>& gemSimHitInput_;
  const edm::EDGetTokenT<edm::PSimHitContainer>& cscSimHitInput_;
  const edm::EDGetTokenT<edm::PSimHitContainer>& rpcSimHitInput_;
  const edm::EDGetTokenT<edm::PSimHitContainer>& me0SimHitInput_;
  const edm::EDGetTokenT<edm::PSimHitContainer>& dtSimHitInput_;



  const auto& gemSimHit_ = conf().getParameter<edm::ParameterSet>("gemSimHit");
  verboseGEM_ = gemSimHit_.getParameter<int>("verbose");
  simMuOnlyGEM_ = gemSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsGEM_ = gemSimHit_.getParameter<bool>("discardEleHits");

  const auto& cscSimHit_= conf().getParameter<edm::ParameterSet>("cscSimHit");
  verboseCSC_ = cscSimHit_.getParameter<int>("verbose");
  simMuOnlyCSC_ = cscSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsCSC_ = cscSimHit_.getParameter<bool>("discardEleHits");

  const auto& me0SimHit_ = conf().getParameter<edm::ParameterSet>("me0SimHit");
  verboseME0_ = me0SimHit_.getParameter<int>("verbose");
  simMuOnlyME0_ = me0SimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsME0_ = me0SimHit_.getParameter<bool>("discardEleHits");

  const auto& rpcSimHit_ = conf().getParameter<edm::ParameterSet>("rpcSimHit");
  verboseRPC_ = rpcSimHit_.getParameter<int>("verbose");
  simMuOnlyRPC_ = rpcSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsRPC_ = rpcSimHit_.getParameter<bool>("discardEleHits");

  const auto& dtSimHit_ = conf().getParameter<edm::ParameterSet>("dtSimHit");
  verboseDT_ = dtSimHit_.getParameter<int>("verbose");
  simMuOnlyDT_ = dtSimHit_.getParameter<bool>("simMuOnly");
  discardEleHitsDT_ = dtSimHit_.getParameter<bool>("discardEleHits");

  verboseSimTrack_ = (verboseCSC_ || verboseGEM_ || verboseME0_ || verboseRPC_ || verboseDT_);
}

MuonHitMatcher::~MuonHitMatcher() {}


}

  /// initialize the event
  init(const edm::Event& e, const edm::EventSetup& eventSetup)
  {
  hasGEMGeometry_ = true;
  hasRPCGeometry_ = true;
  hasCSCGeometry_ = true;
  hasME0Geometry_ = true;
  hasDTGeometry_ = true;

  es.get<MuonGeometryRecord>().get(gem_geom_);
  if (gem_geom_.isValid()) {
    gemGeometry_ = &*gem_geom_;
  } else {
    hasGEMGeometry_ = false;
    if (useGEM)
      std::cout << "+++ Info: GEM geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(me0_geom_);
  if (me0_geom_.isValid()) {
    me0Geometry_ = &*me0_geom_;
  } else {
    hasME0Geometry_ = false;
    if (useCSCChamberTypes_[GEM_ME0]) // if ME0 is still used butME0 geom is not found, print out warning
      std::cout << "+++ Info: ME0 geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(rpc_geom_);
  if (rpc_geom_.isValid()) {
    rpcGeometry_ = &*rpc_geom_;
  } else {
    hasRPCGeometry_ = false;
    std::cout << "+++ Info: RPC geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(dt_geom_);
  if (dt_geom_.isValid()) {
    dtGeometry_ = &*dt_geom_;
  } else {
    hasDTGeometry_ = false;
    std::cout << "+++ Info: DT geometry is unavailable. +++\n";
  }

  es.get<MuonGeometryRecord>().get(csc_geom_);
  if (csc_geom_.isValid()) {
    cscGeometry_ = &*csc_geom_;
  } else {
    hasCSCGeometry_ = false;
    std::cout << "+++ Info: CSC geometry is unavailable. +++\n";
  }


  edm::Handle<edm::PSimHitContainer> dt_hits;
  iEvent.getByToken(dtSimHitInput_, dt_hits);

  edm::Handle<edm::PSimHitContainer> rpc_hits;
  iEvent.getByToken(rpcSimHitInput_, rpc_hits);

  edm::Handle<edm::PSimHitContainer> me0_hits;
  iEvent.getByToken(me0SimHitInput_, me0_hits);

  edm::Handle<edm::PSimHitContainer> gem_hits;
  iEvent.getByToken(gemSimHitInput_, gem_hits);

  edm::Handle<edm::PSimHitContainer> csc_hits;
  iEvent.getByToken(cscSimHitInput_, csc_hits);

}

  /// do the matching
  void match(const SimTrack& t, const SimVertex& v)
  {

  // fill trkId2Index associoation:
  int no = 0;
  trkid_to_index_.clear();
  for (const auto& t: *sim_tracks.product())
  {
    trkid_to_index_[t.trackId()] = no;
    no++;
  }

  vector<unsigned> track_ids = getIdsOfSimTrackShower(trk().trackId(), *sim_tracks.product(), *sim_vertices.product());
  if (verboseSimTrack_) {
    std::cout << "Printing track_ids" << std::endl;
    for (const auto& id: track_ids) std::cout << "id: " << id << std::endl;
  }

  if (hasCSCGeometry_) {

      // select CSC simhits
      edm::PSimHitContainer csc_hits_select;
      for (const auto& h: *csc_hits.product()) {
        const CSCDetId& id(h.detUnitId());
        if (useCSCChamberType(gemvalidation::toCSCType(id.station(), id.ring()))) csc_hits_select.push_back(h);
      }

      if(runCSCSimHit_) {
        matchCSCSimHitsToSimTrack(track_ids, csc_hits_select);

	if (verboseCSC_) {
	  cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedCSCSimHits "<<csc_hits_select.size()<<endl;
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
    }
  }

  if (hasGEMGeometry_) {

      // select GEM simhits
      edm::PSimHitContainer gem_hits_select;
      for (const auto& h: *gem_hits.product()) {
        const GEMDetId& id(h.detUnitId());
        if (useGEMChamberType(gemvalidation::toGEMType(id.station(), id.ring()))) gem_hits_select.push_back(h);
      }

        matchGEMSimHitsToSimTrack(track_ids, gem_hits_select);

        if (verboseGEM_) {
          cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedGEMSimHits "<<(*gem_hits.product()).size()<<endl;
          cout << "detids GEM " << detIdsGEM().size() << endl;

          const auto& gem_ch_ids = detIdsGEM();
          for (const auto& id: gem_ch_ids) {
            const auto& gem_simhits = hitsInDetId(id);
            const auto& gem_simhits_gp = simHitsMeanPosition(gem_simhits);
            cout<<"gemchid "<<GEMDetId(id)<<": nHits "<<gem_simhits.size()<<" phi "<<gem_simhits_gp.phi()<<" nCh "<< gem_chamber_to_hits_[id].size()<<endl;
            // const auto& strips = hitStripsInDetId(id);
            // cout<<"nStrip "<<strips.size()<<endl;
            // cout<<"strips : "; std::copy(strips.begin(), strips.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
          }
          const auto& gem_sch_ids = superChamberIdsGEM();
          for (const auto& id: gem_sch_ids) {
            const auto& gem_simhits = hitsInSuperChamber(id);
            const auto& gem_simhits_gp = simHitsMeanPosition(gem_simhits);
            cout<<"gemschid "<<GEMDetId(id)<<": "<<nCoincidencePadsWithHits() <<" | "<<gem_simhits.size()<<" "<<gem_simhits_gp.phi()<<" "<< gem_superchamber_to_hits_[id].size()<<endl;
          }
      }
    }
  }

  if (hasME0Geometry_) {

        matchME0SimHitsToSimTrack(track_ids, *me0_hits.product());

        if (verboseME0_) {
          cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" ME0SimHits "<<(*me0_hits.product()).size()<<endl;
          cout << "detids ME0 " << detIdsME0().size() << endl;

          const auto& me0_ch_ids = detIdsME0();
          for (const auto& id: me0_ch_ids) {
            const auto& me0_simhits = hitsInChamber(id);
            const auto& me0_simhits_gp = simHitsMeanPosition(me0_simhits);
            cout<<"me0chid "<<ME0DetId(id)<<": nHits "<<me0_simhits.size()<<" phi "<<me0_simhits_gp.phi()<<" nCh "<< me0_chamber_to_hits_[id].size()<<endl;
            // const auto& strips = hitStripsInDetId(id);
            // cout<<"nStrip "<<strips.size()<<endl;
            // cout<<"strips : "; std::copy(strips.begin(), strips.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
          }
      }
    }
  }

  if (hasRPCGeometry_) {

      // select RPC simhits
      edm::PSimHitContainer rpc_hits_select;
      for (const auto& h: *rpc_hits.product()) {
        const RPCDetId& id(h.detUnitId());
        if (useRPCChamberType(gemvalidation::toRPCType(id.region(), id.station(), id.ring()))) rpc_hits_select.push_back(h);
      }

        matchRPCSimHitsToSimTrack(track_ids, rpc_hits_select);

        if (verboseRPC_) {
          cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedRPCSimHits "<<(*rpc_hits.product()).size()<<endl;
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
  }

  if (hasDTGeometry_) {

      // select DT simhits
      edm::PSimHitContainer dt_hits_select;
      for (const auto& h: *dt_hits.product()) {
        const DTWireId id(h.detUnitId());
        if (useDTChamberType(gemvalidation::toDTType(id.wheel(), id.station()))) dt_hits_select.push_back(h);
      }

        matchDTSimHitsToSimTrack(track_ids, dt_hits_select);

        if (verboseDT_) {
          cout<<"nSimHits "<<no<<" nTrackIds "<<track_ids.size()<<" nSelectedDTSimHits "<<(*dt_hits.product()).size()<<endl;
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

}

std::vector<unsigned int>
MuonHitMatcher::getIdsOfSimTrackShower(unsigned int initial_trk_id,
    const edm::SimTrackContainer & sim_tracks, const edm::SimVertexContainer & sim_vertices)
{
  vector<unsigned int> result;
  result.push_back(initial_trk_id);

  if (! (simMuOnlyGEM_ || simMuOnlyCSC_ || simMuOnlyDT_ || simMuOnlyME0_ || simMuOnlyRPC_) ) return result;

  for (const auto& t: sim_tracks)
  {
    SimTrack last_trk = t;
    //if (std::abs(t.type()) != 13) continue;
    bool is_child = 0;
    while (1)
    {
      if ( last_trk.noVertex() ) break;
      if ( sim_vertices[last_trk.vertIndex()].noParent() ) break;

      unsigned parentId = sim_vertices[last_trk.vertIndex()].parentIndex();
      if ( parentId == initial_trk_id )
      {
        is_child = 1;
        break;
      }

      const auto& association = trkid_to_index_.find( parentId );
      if ( association == trkid_to_index_.end() ) break;

      last_trk = sim_tracks[ association->second ];
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
      if (gemvalidation::isCSC(h.detUnitId()))
       {
     	 gp = getCSCGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
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
    const auto& roll = getGEMGeometry()->etaPartition(id);
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
    const auto& roll1 = getGEMGeometry()->etaPartition(id1);
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
      const auto& roll2 = getGEMGeometry()->etaPartition(id2);
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
MuonHitMatcher::simHits(enum MuonType sub) const
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
      if (gemvalidation::toGEMType(detId.station(), detId.ring()) != gem_type) continue;
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
      if (gemvalidation::toRPCType(detId.region(), detId.station(), detId.ring()) != rpc_type) continue;
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
      if (gemvalidation::toCSCType(detId.station(), detId.ring()) != csc_type) continue;
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
      if (gemvalidation::toDTType(detId.wheel(), detId.station()) != dt_type) continue;
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
      if (gemvalidation::toGEMType(detId.station(), detId.ring()) != gem_type) continue;
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
      if (gemvalidation::toRPCType(detId.region(), detId.station(), detId.ring()) != rpc_type) continue;
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
      if (gemvalidation::toCSCType(detId.station(), detId.ring()) != csc_type) continue;
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
      if (gemvalidation::toDTType(detId.wheel(), detId.station()) != dt_type) continue;
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
  if (gemvalidation::isGEM(detid))
  {
    if (gem_detid_to_hits_.find(detid) == gem_detid_to_hits_.end()) return no_hits_;
    return gem_detid_to_hits_.at(detid);
  }
  if (gemvalidation::isME0(detid))
  {
    if (me0_detid_to_hits_.find(detid) == me0_detid_to_hits_.end()) return no_hits_;
    return me0_detid_to_hits_.at(detid);
  }
  if (gemvalidation::isCSC(detid))
  {
    if (csc_detid_to_hits_.find(detid) == csc_detid_to_hits_.end()) return no_hits_;
    return csc_detid_to_hits_.at(detid);
  }
  if (gemvalidation::isRPC(detid))
  {
    if (rpc_detid_to_hits_.find(detid) == rpc_detid_to_hits_.end()) return no_hits_;
    return rpc_detid_to_hits_.at(detid);
  }
  if(gemvalidation::isDT(detid))
  {
    if (dt_detid_to_hits_.find(detid) == dt_detid_to_hits_.end()) return no_hits_;
    return dt_detid_to_hits_.at(detid);
  }
  return no_hits_;
}


const edm::PSimHitContainer&
MuonHitMatcher::hitsInChamber(unsigned int detid) const
{
  if (gemvalidation::isGEM(detid)) // make sure we use chamber id
  {
    const GEMDetId id(detid);
    if (gem_chamber_to_hits_.find(id.chamberId().rawId()) == gem_chamber_to_hits_.end()) return no_hits_;
    return gem_chamber_to_hits_.at(id.chamberId().rawId());
  }
  if (gemvalidation::isME0(detid)) // make sure we use chamber id
  {
    const ME0DetId id(detid);
    if (me0_chamber_to_hits_.find(id.layerId().rawId()) == me0_chamber_to_hits_.end()) return no_hits_;
    return me0_chamber_to_hits_.at(id.layerId().rawId());
  }
  if (gemvalidation::isCSC(detid))
  {
    const CSCDetId id(detid);
    if (csc_chamber_to_hits_.find(id.chamberId().rawId()) == csc_chamber_to_hits_.end()) return no_hits_;
    return csc_chamber_to_hits_.at(id.chamberId().rawId());
  }
  if (gemvalidation::isRPC(detid))
  {
    const RPCDetId id(detid);
    if (rpc_chamber_to_hits_.find(id.chamberId().rawId()) == rpc_chamber_to_hits_.end()) return no_hits_;
    return rpc_chamber_to_hits_.at(id.chamberId().rawId());
  }
  if(gemvalidation::isDT(detid))
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
  if (gemvalidation::isGEM(detid))
  {
    const GEMDetId id(detid);
    if (gem_superchamber_to_hits_.find(id.chamberId().rawId()) == gem_superchamber_to_hits_.end()) return no_hits_;
    return gem_superchamber_to_hits_.at(id.chamberId().rawId());
  }
  if (gemvalidation::isCSC(detid)) return hitsInChamber(detid);
  if (gemvalidation::isME0(detid))
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
  if (!gemvalidation::isDT(detid)) return no_hits_;

  const DTWireId id(detid);
  if (dt_layer_to_hits_.find(id.layerId().rawId()) == dt_layer_to_hits_.end()) return no_hits_;
  return dt_layer_to_hits_.at(id.layerId().rawId());
}

const edm::PSimHitContainer&
MuonHitMatcher::hitsInSuperLayerDT(unsigned int detid) const
{
  if (!gemvalidation::isDT(detid)) return no_hits_;

  const DTWireId id(detid);
  if (dt_superlayer_to_hits_.find(id.superlayerId().rawId()) == dt_superlayer_to_hits_.end()) return no_hits_;
  return dt_superlayer_to_hits_.at(id.superlayerId().rawId());
}

const edm::PSimHitContainer&
MuonHitMatcher::hitsInChamberDT(unsigned int detid) const
{
  if (!gemvalidation::isDT(detid)) return no_hits_;

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
    if (gemvalidation::isGEM(detid))
    {
      const GEMDetId& idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
    if (gemvalidation::isME0(detid))
    {
      const ME0DetId& idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
    if (gemvalidation::isCSC(detid))
    {
      const CSCDetId& idd(h.detUnitId());
      layers_with_hits.insert(idd.layer());
    }
    if (gemvalidation::isRPC(detid))
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
    if (gemvalidation::isDT(detid)) {
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
    if (gemvalidation::isDT(detid)) {
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
    if (gemvalidation::isDT(detid)) {
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
  const auto& superLayers(getDTGeometry()->chamber(DTChamberId(detid))->superLayers());
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
    if ( gemvalidation::isGEM(h.detUnitId()) )
    {
      gp = getGEMGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if ( gemvalidation::isME0(h.detUnitId()) )
    {
      gp = getME0Geometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if (gemvalidation::isCSC(h.detUnitId()))
    {
      gp = getCSCGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if (gemvalidation::isRPC(h.detUnitId()))
    {
      gp = getRPCGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if (gemvalidation::isDT(h.detUnitId()))
    {
      gp = getDTGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
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

GlobalPoint
MuonHitMatcher::simHitPositionKeyLayer(unsigned int chid) const
{
  /*
    1. Return the (average) position of the keylayer simhit if available
    2. Return the fitted position of the simhit in the keylayer if fit good
    3. Return average position of the simhits in the chamber if all else fails
  */
  const CSCDetId chamberId(chid);
  const CSCDetId keyLayerId(chamberId.endcap(), chamberId.station(),
                            chamberId.ring(), chamberId.chamber(), 3);
  GlobalPoint returnValue;

  const edm::PSimHitContainer& keyLayerIdHits(hitsInDetId(keyLayerId.rawId()));
  //std::cout <<"CSC id "<< chamberId <<" keyLayerIdHits size "<< keyLayerIdHits.size() << " hitsInChamber "<< hitsInChamber(chid).size()<< std::endl;
  if (keyLayerIdHits.size()!=0) returnValue = simHitsMeanPosition(keyLayerIdHits);
  else{
    // check if the chamber has hits at all
    if (hitsInChamber(chid).size()==0) returnValue = GlobalPoint();
    //else if (hitsInChamber(chid).size()>=1) returnValue = simHitsMeanPosition(hitsInChamber(chid));
    else if (hitsInChamber(chid).size()==1) returnValue = simHitsMeanPosition(hitsInChamber(chid));
    else if (hitsInChamber(chid).size()==2) returnValue = simHitsMeanPosition(hitsInChamber(chid));
    else {
      std::vector<float> zs;
      std::vector<float> xs;
      std::vector<float> ys;
      std::vector<float> ezs;
      std::vector<float> exs;
      std::vector<float> eys;
      std::vector<float> status;

      // add the positions of all hits in a chamber
      for (int l=1; l<=6; l++){
        CSCDetId l_id(chamberId.endcap(), chamberId.station(),
                      chamberId.ring(), chamberId.chamber(), l);
        for (const auto& p: hitsInDetId(l_id.rawId())) {
	  //std::cout <<"layerId "<< l_id;
          const LocalPoint& lp = p.entryPoint();
	  //std::cout <<" localPoint (x,y): "<<lp.x()<<" "<< lp.y();
          const GlobalPoint& gp = getCSCGeometry()->idToDet(p.detUnitId())->surface().toGlobal(lp);
	 // std::cout <<" gp (x,y,z) "<< gp.x()<<" "<< gp.y()<<" "<< gp.z()<< std::endl;
          zs.push_back(gp.z());
          xs.push_back(gp.x());
          ys.push_back(gp.y());
	  ezs.push_back(0);
	  exs.push_back(0);
	  eys.push_back(0);
        }
      }
      // fit a straight line through the hits (bending is negligible
      //std::cout <<"size "<<zs.size()<<" xs[0] "<< xs[0] <<" ys[0] "<< ys[0]<<" zs[0] "<< zs[0] << std::endl;
      float alphax = -99., betax = 0.;
      PtassignmentHelper::calculateAlphaBeta(zs, xs, ezs, exs, status,
     		     alphax, betax);
      float alphay = -99., betay = 0.;
      PtassignmentHelper::calculateAlphaBeta(zs, ys, ezs, exs, status,
    		     alphay, betay);
      if(std::abs(betax)>0.0 and std::abs(betay)>0.0){
        float z_pos_L3 = getCSCGeometry()->layer(keyLayerId)->centerOfStrip(20).z();
        float x_pos_L3 = alphax + betax * z_pos_L3;
        float y_pos_L3 = alphay + betay * z_pos_L3;
        returnValue =  GlobalPoint(x_pos_L3, y_pos_L3, z_pos_L3);
	//std::cout <<"return gp of keylayer at sim level, id "<<  chamberId <<" z "<< z_pos_L3 <<" perp "<< returnValue.perp()<< std::endl;
      }
      else{
        returnValue = simHitsMeanPosition(hitsInChamber(chid));
      }
    }
  }

	//GlobalPoint gptest(simHitsMeanPosition(hitsInChamber(chid)));
	//std::cout <<"as comparison, gp from MeanPosition, id  "<< chamberId <<" z "<< gptest.z()<<" perp "<< gptest.perp() << std::endl;
  //std::cout <<"final return gp of keylayer at sim level, id "<< chamberId <<" z "<< returnValue.z() <<" perp "<< returnValue.perp() <<" eta "<< returnValue.eta()<<" phi "<< returnValue.phi() << std::endl;
  return returnValue;
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
    if ( gemvalidation::isGEM(h.detUnitId()) )
    {
      gv = getGEMGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if ( gemvalidation::isME0(h.detUnitId()) )
    {
      gv = getME0Geometry()->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if (gemvalidation::isCSC(h.detUnitId()))
    {
      gv = getCSCGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if (gemvalidation::isRPC(h.detUnitId()))
    {
      gv = getRPCGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lv);
    }
    else if (gemvalidation::isDT(h.detUnitId()))
    {
      gv = getDTGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lv);
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
    if ( gemvalidation::isGEM(h.detUnitId()) )
    {
      gp = getGEMGeometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
    }
    else if ( gemvalidation::isME0(h.detUnitId()) )
    {
      gp = getME0Geometry()->idToDet(h.detUnitId())->surface().toGlobal(lp);
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
    if ( gemvalidation::isGEM(d) )
    {
      s = getGEMGeometry()->etaPartition(d)->strip(lp);
    }
    else if ( gemvalidation::isME0(d) )
    {
      s = getME0Geometry()->etaPartition(d)->strip(lp);
    }
    else if (gemvalidation::isCSC(d))
    {
      s = getCSCGeometry()->layer(d)->geometry()->strip(lp);
      // convert to half-strip:
      s *= 2.;
    }
    else if (gemvalidation::isRPC(d))
    {
      s = getRPCGeometry()->roll(d)->strip(lp);
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
    if (gemvalidation::isCSC(d))
    {
      // find nearest wire
      int nearestWire(getCSCGeometry()->layer(d)->geometry()->nearestWire(lp));
      // then find the corresponding wire group
      s = getCSCGeometry()->layer(d)->geometry()->wireGroup(nearestWire);
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
    if (gemvalidation::isDT(d))
    {
      // find nearest wire
      s  = getDTGeometry()->layer(DTLayerId(d))->specificTopology().channel(lp);
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
  if ( gemvalidation::isGEM(detid) )
  {
    GEMDetId id(detid);
    int max_nstrips = getGEMGeometry()->etaPartition(id)->nstrips();
    for (const auto& h: simhits)
    {
      const LocalPoint& lp = h.entryPoint();
      int central_strip = static_cast<int>(getGEMGeometry()->etaPartition(id)->topology().channel(lp));
      int smin = central_strip - margin_n_strips;
      smin = (smin > 0) ? smin : 1;
      int smax = central_strip + margin_n_strips;
      smax = (smax <= max_nstrips) ? smax : max_nstrips;
      for (int ss = smin; ss <= smax; ++ss) result.insert(ss);
    }
  }
  else if ( gemvalidation::isME0(detid) )
  {
    ME0DetId id(detid);
    int max_nstrips = getME0Geometry()->etaPartition(id)->nstrips();
    for (const auto& h: simhits)
    {
      const LocalPoint& lp = h.entryPoint();
      int central_strip = 1 + static_cast<int>(getME0Geometry()->etaPartition(id)->topology().channel(lp));
      int smin = central_strip - margin_n_strips;
      smin = (smin > 0) ? smin : 1;
      int smax = central_strip + margin_n_strips;
      smax = (smax <= max_nstrips) ? smax : max_nstrips;
      for (int ss = smin; ss <= smax; ++ss) result.insert(ss);
    }
  }
  else if ( gemvalidation::isCSC(detid) )
  {
    CSCDetId id(detid);
    int max_nstrips = getCSCGeometry()->layer(id)->geometry()->numberOfStrips();
    for (const auto& h: simhits)
    {
      const LocalPoint& lp = h.entryPoint();
      int central_strip = getCSCGeometry()->layer(id)->geometry()->nearestStrip(lp);
      int smin = central_strip - margin_n_strips;
      smin = (smin > 0) ? smin : 1;
      int smax = central_strip + margin_n_strips;
      smax = (smax <= max_nstrips) ? smax : max_nstrips;
      for (int ss = smin; ss <= smax; ++ss) result.insert(ss);
    }
  }
  else if ( gemvalidation::isRPC(detid) )
  {
    RPCDetId id(detid);
    for (const auto& roll: getRPCGeometry()->chamber(id)->rolls()) {
      int max_nstrips = roll->nstrips();
      for (const auto& h: hitsInDetId(roll->id().rawId())) {
        const LocalPoint& lp = h.entryPoint();
        // check how the RPC strip numbers start counting - Ask Piet!!!
        int central_strip = static_cast<int>(roll->topology().channel(lp));
        // int central_strip2 = 1 + static_cast<int>(getRPCGeometry()->roll(id)->strip(lp));
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
  if ( gemvalidation::isCSC(detid) ) {
    const auto& simhits = hitsInDetId(detid);
    CSCDetId id(detid);
    int max_n_wg = getCSCGeometry()->layer(id)->geometry()->numberOfWireGroups();
    for (const auto& h: simhits) {
      const LocalPoint& lp = h.entryPoint();
      const auto& layer_geo = getCSCGeometry()->layer(id)->geometry();
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
  if ( gemvalidation::isDT(detid) ) {
    DTLayerId id(detid);
    int max_nwires = getDTGeometry()->layer(id)->specificTopology().channels();
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
  const auto& layers(getDTGeometry()->superLayer(DTSuperLayerId(detid))->layers());
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
  const auto& superLayers(getDTGeometry()->chamber(DTChamberId(detid))->superLayers());
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
    const auto& p1(chamberIdsCSC(CSC_ME1a));
    const auto& p2(chamberIdsCSC(CSC_ME1b));
    const auto& p3(chamberIdsCSC(CSC_ME12));
    const auto& p4(chamberIdsCSC(CSC_ME13));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    break;
  }
  case 2: {
    const auto& p1(chamberIdsCSC(CSC_ME21));
    const auto& p2(chamberIdsCSC(CSC_ME22));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    break;
  }
  case 3: {
    const auto& p1(chamberIdsCSC(CSC_ME31));
    const auto& p2(chamberIdsCSC(CSC_ME32));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    break;
  }
  case 4: {
    const auto& p1(chamberIdsCSC(CSC_ME41));
    const auto& p2(chamberIdsCSC(CSC_ME42));
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
    const auto& p1(chamberIdsDT(DT_MB21p));
    const auto& p2(chamberIdsDT(DT_MB11p));
    const auto& p3(chamberIdsDT(DT_MB01));
    const auto& p4(chamberIdsDT(DT_MB11n));
    const auto& p5(chamberIdsDT(DT_MB21n));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    result.insert(p5.begin(),p5.end());
    break;
  }
  case 2: {
    const auto& p1(chamberIdsDT(DT_MB22p));
    const auto& p2(chamberIdsDT(DT_MB12p));
    const auto& p3(chamberIdsDT(DT_MB02));
    const auto& p4(chamberIdsDT(DT_MB12n));
    const auto& p5(chamberIdsDT(DT_MB22n));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    result.insert(p5.begin(),p5.end());
    break;
  }
  case 3: {
    const auto& p1(chamberIdsDT(DT_MB23p));
    const auto& p2(chamberIdsDT(DT_MB13p));
    const auto& p3(chamberIdsDT(DT_MB03));
    const auto& p4(chamberIdsDT(DT_MB13n));
    const auto& p5(chamberIdsDT(DT_MB23n));
    result.insert(p1.begin(),p1.end());
    result.insert(p2.begin(),p2.end());
    result.insert(p3.begin(),p3.end());
    result.insert(p4.begin(),p4.end());
    result.insert(p5.begin(),p5.end());
    break;
  }
  case 4: {
    const auto& p1(chamberIdsDT(DT_MB24p));
    const auto& p2(chamberIdsDT(DT_MB14p));
    const auto& p3(chamberIdsDT(DT_MB04));
    const auto& p4(chamberIdsDT(DT_MB14n));
    const auto& p5(chamberIdsDT(DT_MB24n));
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
