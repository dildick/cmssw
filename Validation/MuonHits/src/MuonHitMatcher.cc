#include "Validation/MuonHits/interface/MuonHitMatcher.h"

#include <algorithm>

using namespace std;

MuonHitMatcher::MuonHitMatcher(const edm::ParameterSet& ps, edm::ConsumesCollector && iC)
{
  const auto& simVertex = ps.getParameterSet("simVertex");
  const auto& simTrack = ps.getParameterSet("simTrack");
  verboseSimTrack_ = simTrack.getParameter<int>("verbose");

  simVertexInput_ = iC.consumes<edm::SimVertexContainer>(simVertex.getParameter<edm::InputTag>("inputTag"));
  simTrackInput_ = iC.consumes<edm::SimTrackContainer>(simTrack.getParameter<edm::InputTag>("inputTag"));
}

/// initialize the event
void MuonHitMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  hasGeometry_ = true;

  iEvent.getByToken(simTrackInput_, simTracksH_);
  iEvent.getByToken(simVertexInput_, simVerticesH_);
  iEvent.getByToken(simHitInput_, simHitsH_);
}

/// do the matching
void MuonHitMatcher::match(const SimTrack& track, const SimVertex& vertex)
{
  simTracks_ = *simTracksH_.product();
  simVertices_ = *simVerticesH_.product();
  simHits_ = *simHitsH_.product();

  // fill trkId2Index association:
  int no = 0;
  trkid_to_index_.clear();
  for (const auto& t: simTracks_) {
    trkid_to_index_[t.trackId()] = no;
    no++;
  }

  vector<unsigned> track_ids = getIdsOfSimTrackShower(track.trackId(), simTracks_, simVertices_);
  if (verboseSimTrack_) {
    std::cout << "Printing track_ids" << std::endl;
    for (const auto& id: track_ids) std::cout << "id: " << id << std::endl;
  }
}

std::vector<unsigned int>
MuonHitMatcher::getIdsOfSimTrackShower(unsigned int initial_trk_id,
    const edm::SimTrackContainer & simTracks, const edm::SimVertexContainer & simVertices)
{
  vector<unsigned int> result;
  result.push_back(initial_trk_id);

  if (!simMuOnly_) return result;

  for (const auto& t: simTracks_)
  {
    SimTrack last_trk = t;
    //if (std::abs(t.type()) != 13) continue;
    bool is_child = 0;
    while (1)
    {
      if ( last_trk.noVertex() ) break;
      if ( simVertices_[last_trk.vertIndex()].noParent() ) break;

      unsigned parentId = simVertices_[last_trk.vertIndex()].parentIndex();
      if ( parentId == initial_trk_id )
      {
        is_child = 1;
        break;
      }

      const auto& association = trkid_to_index_.find( parentId );
      if ( association == trkid_to_index_.end() ) break;

      last_trk = simTracks_[ association->second ];
    }
    if (is_child)
    {
      result.push_back(t.trackId());
    }
  }
  return result;
}

const edm::PSimHitContainer&
MuonHitMatcher::simHits(int sub) const
{
  return hits_;
}

const edm::PSimHitContainer&
MuonHitMatcher::hitsInDetId(unsigned int detid) const
{
  if (detid_to_hits_.find(detid) == detid_to_hits_.end()) return no_hits_;
  return detid_to_hits_.at(detid);
}


const edm::PSimHitContainer&
MuonHitMatcher::hitsInChamber(unsigned int detid) const
{
  if (chamber_to_hits_.find(detid) == chamber_to_hits_.end()) return no_hits_;
  return chamber_to_hits_.at(detid);
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
    const GlobalPoint& gp = geometry_->idToDet(h.detUnitId())->surface().toGlobal(lp);
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
    const GlobalVector& gv = geometry_->idToDet(h.detUnitId())->surface().toGlobal(lv);
    sumx += gv.x();
    sumy += gv.y();
    sumz += gv.z();
    ++n;
  }
  if (n == 0) return GlobalVector();
  return GlobalVector(sumx/n, sumy/n, sumz/n);
}

