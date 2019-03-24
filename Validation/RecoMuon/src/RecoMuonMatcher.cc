#include "Validation/RecoMuon/interface/RecoMuonMatcher.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "TLorentzVector.h"
#include <map>

RecoMuonMatcher::RecoMuonMatcher(const edm::ParameterSet& pset, edm::ConsumesCollector && iC)
{
  const auto& recoTrackExtra = pset.getParameterSet("recoTrackExtra");
  verboseRecoTrackExtra_ = recoTrackExtra.getParameter<int>("verbose");

  const auto& recoTrack = pset.getParameterSet("recoTrack");
  verboseRecoTrack_ = recoTrack.getParameter<int>("verbose");

  const auto& recoChargedCandidate = pset.getParameterSet("recoChargedCandidate");
  verboseRecoChargedCandidate_ = recoChargedCandidate.getParameter<int>("verbose");

  // make a new simhits matcher
  gemRecHitMatcher_.reset(new GEMRecHitMatcher(pset, std::move(iC)));
  me0RecHitMatcher_.reset(new ME0RecHitMatcher(pset, std::move(iC)));
  dtRecHitMatcher_.reset(new DTRecHitMatcher(pset, std::move(iC)));
  rpcRecHitMatcher_.reset(new RPCRecHitMatcher(pset, std::move(iC)));
  cscRecHitMatcher_.reset(new CSCRecHitMatcher(pset, std::move(iC)));

  edm::InputTag recoTrackExtraTag = recoTrackExtra.getParameter<edm::InputTag>("inputTag");
  edm::InputTag recoTrackTag = recoTrack.getParameter<edm::InputTag>("inputTag");
  edm::InputTag recoChargedCandidateTag = recoChargedCandidate.getParameter<edm::InputTag>("inputTag");

  recoTrackExtraToken_ = iC.consumes<reco::TrackExtraCollection>(recoTrackExtraTag);
  recoTrackToken_ = iC.consumes<reco::TrackCollection>(recoTrackTag);
  recoChargedCandidateToken_ = iC.consumes<reco::RecoChargedCandidateCollection>(recoChargedCandidateTag);
}

void RecoMuonMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  gemRecHitMatcher_->init(iEvent, iSetup);
  me0RecHitMatcher_->init(iEvent, iSetup);
  dtRecHitMatcher_->init(iEvent, iSetup);
  rpcRecHitMatcher_->init(iEvent, iSetup);
  cscRecHitMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(recoTrackExtraToken_, recoTrackExtraH_);
  iEvent.getByToken(recoTrackToken_, recoTrackH_);
  iEvent.getByToken(recoChargedCandidateToken_, recoChargedCandidateH_);
}

/// do the matching
void RecoMuonMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match rechits and segments first
  gemRecHitMatcher_->match(t,v);
  me0RecHitMatcher_->match(t,v);
  dtRecHitMatcher_->match(t,v);
  rpcRecHitMatcher_->match(t,v);
  cscRecHitMatcher_->match(t,v);

  matchRecoTrackExtraToSimTrack(t, *recoTrackExtraH_.product());
  matchRecoTrackToSimTrack(t, *recoTrackH_.product());
  matchRecoChargedCandidateToSimTrack(t, *recoChargedCandidateH_.product());
}

void
RecoMuonMatcher::matchRecoTrackExtraToSimTrack(const SimTrack& trk, const reco::TrackExtraCollection& tracks)
{
  if (verboseRecoTrackExtra_) std::cout << "Number of RecoTrackExtras: " <<tracks.size() << std::endl;
  for(const auto& track: tracks) {
    // do not anlyze tracks with large deltaR
    if (reco::deltaR(track.innerPosition(), trk.momentum()) > 0.5) continue;
    if (verboseRecoTrackExtra_) {
      std::cout << "RecoTrackExtra " << std::endl
                << "\tpT_inner: "<<track.innerMomentum().Rho()
                << ", eta_inner: "<<track.innerPosition().eta()
                << ", phi_inner: "<<track.innerPosition().phi()
                << "\tpT_outer: "<<track.outerMomentum().Rho()
                << ", eta_outer: "<<track.outerPosition().eta()
                << ", phi_outer: "<<track.outerPosition().phi()
                <<std::endl;
      std::cout << "\tDeltaR(SimTrack, RecoTrackExtra): " << reco::deltaR(track.innerPosition(), trk.momentum()) << std::endl;
      std::cout << "\tDeltaPt(SimTrack, RecoTrackExtra): " << std::fabs(track.innerMomentum().Rho()-trk.momentum().pt()) << std::endl;
      std::cout << "\tRechits/Segments: " << track.recHitsSize()<< std::endl;
    }
    int matchingCSCSegments(0);
    int matchingRPCSegments(0);
    int matchingGEMSegments(0);
    int matchingDTSegments(0);
    int matchingSegments(0);
    int nValidSegments(0);
    for(auto rh = track.recHitsBegin(); rh != track.recHitsEnd(); rh++) {
      if (!(**rh).isValid()) continue;
      ++nValidSegments;
      const auto& id((**rh).rawId());
      if (MuonHitHelper::isDT(id)) {
        const DTRecSegment4D *seg = dynamic_cast<const DTRecSegment4D*>(*rh);
        if (verboseRecoTrackExtra_) {
          std::cout << "\t\tDT  :: id :: " << DTChamberId(id) << std::endl;
          std::cout << "\t\t    :: segment :: " << *seg << std::endl;
        }
        if (dtRecHitMatcher_->isDTRecSegment4DMatched(*seg)) {
          if (verboseRecoTrackExtra_) std::cout << "\t\t    :: MATCHED!" << std::endl;
          ++matchingDTSegments;
          ++matchingSegments;
        }
      }
      if (MuonHitHelper::isRPC(id)) {
        const RPCRecHit* rpcrh = dynamic_cast<const RPCRecHit*>(*rh);
        if (verboseRecoTrackExtra_) {
          std::cout << "\t\tRPC :: id :: " << RPCDetId(id) << std::endl;
          std::cout << "\t\t    :: rechit :: " << *rpcrh << std::endl;
        }
        if (rpcRecHitMatcher_->isRPCRecHitMatched(*rpcrh)) {
          if (verboseRecoTrackExtra_) std::cout << "\t\t    :: MATCHED!" << std::endl;
          ++matchingRPCSegments;
          ++matchingSegments;
        }
      }
      if (MuonHitHelper::isGEM(id)) {
        const GEMRecHit* gemrh = dynamic_cast<const GEMRecHit*>(*rh);
        if (verboseRecoTrackExtra_) {
          std::cout << "\t\tGEM :: id :: " << GEMDetId(id) << std::endl;
          std::cout << "\t\t    :: rechit :: " << *gemrh << std::endl;
        }
        if (gemRecHitMatcher_->isGEMRecHitMatched(*gemrh)) {
          if (verboseRecoTrackExtra_) std::cout << "\t\t    :: MATCHED!" << std::endl;
          ++matchingGEMSegments;
          ++matchingSegments;
        }
      }
      if (MuonHitHelper::isCSC(id)) {
        const CSCSegment *seg = dynamic_cast<const CSCSegment*>(*rh);
        if (verboseRecoTrackExtra_) {
          std::cout << "\t\tCSC :: id :: " << CSCDetId(id) << std::endl;
          std::cout << "\t\t    :: segment :: " << *seg << std::endl;
        }
        if (cscRecHitMatcher_->isCSCSegmentMatched(*seg)) {
          if (verboseRecoTrackExtra_) std::cout << "\t\t    :: MATCHED!" << std::endl;
          ++matchingCSCSegments;
          ++matchingSegments;
        }
      }
    }
    if (verboseRecoTrackExtra_) {
      std::cout << "\tValid Segments:    " << nValidSegments << std::endl;
      std::cout << "\tMatching Segments: " << matchingSegments << std::endl;
      std::cout << "\t              RPC: " << matchingRPCSegments << std::endl;
      std::cout << "\t              CSC: " << matchingCSCSegments << std::endl;
      std::cout << "\t              GEM: " << matchingGEMSegments << std::endl;
      std::cout << "\t               DT: " << matchingDTSegments << std::endl;
    }
    // store matching L1RecoTrackExtra
    if (matchingDTSegments + matchingCSCSegments>=2) {
      if (verboseRecoTrackExtra_) {
        std::cout << "\tRecoTrackExtra was matched! (deltaR = " << reco::deltaR(track.innerPosition(), trk.momentum()) << ") " << std::endl;
      }
      recoTrackExtras_.push_back(track);
    }
  }
}


void
RecoMuonMatcher::matchRecoTrackToSimTrack(const SimTrack& trk, const reco::TrackCollection& tracks)
{
  if (verboseRecoTrack_) std::cout << "Number of RecoTracks: " <<tracks.size() << std::endl;
  int i=0;
  for(const auto& track: tracks) {
    const double deltaR(reco::deltaR(track.outerEta(), track.outerPhi(), trk.momentum().eta(), trk.momentum().phi()));
    if (verboseRecoTrack_) {
      std::cout<< "RecoTrack " << i+1 << " - pT: "<<track.outerPt()
               <<", eta: "<<track.outerEta()
               <<", phi: "<<track.outerPhi()
               <<", deltaR: "<< deltaR << std::endl;
    }
    // check if the associated RecoTrackExtra was matched!
    for (const auto& otherTrackExtra: getRecoTrackExtras()) {
      if (areRecoTrackSame(*(track.extra()), otherTrackExtra)) {
        if (verboseRecoTrack_) std::cout << "\tRecoTrack was matched!" << std::endl;
        recoTracks_.push_back(track);
      }
    }
    ++i;
  }
}


void
RecoMuonMatcher::matchRecoChargedCandidateToSimTrack(const SimTrack& trk, const reco::RecoChargedCandidateCollection& candidates)
{
  if (verboseRecoTrack_) std::cout << "Number of RecoChargedCandidates: " <<candidates.size() << std::endl;
  int i=0;
  for(const auto& candidate: candidates) {
    const double deltaR(reco::deltaR(candidate.eta(), candidate.phi(), trk.momentum().eta(), trk.momentum().phi()));
    if (verboseRecoChargedCandidate_) {
      std::cout<< "RecoCandidate " << i+1 << " - pT: "<<candidate.pt()
               <<", eta: "<<candidate.eta()
               <<", phi: "<<candidate.phi()
               <<", deltaR: "<< deltaR << std::endl;
    }
    // get the RecoTrack
    for (const auto& otherTrack: getRecoTracks()) {
      if (areRecoTrackSame(*(candidate.track()), otherTrack)) {
        if (verboseRecoTrack_) std::cout << "\tRecoChargedCandidate was matched!" << std::endl;
        recoChargedCandidates_.push_back(candidate);
      }
    }
    ++i;
  }
}


template<typename T>
bool
RecoMuonMatcher::areRecoTrackSame(const T& l, const T& r) const
{
  return (l.outerPosition()==r.outerPosition() and
          l.outerMomentum()==r.outerMomentum() and
          l.outerDetId()==r.outerDetId() and
          l.outerDetId()==r.outerDetId() and
          l.innerPosition()==r.innerPosition() and
          l.innerMomentum()==r.innerMomentum() and
          l.innerOk()==r.innerOk() and
          l.outerDetId()==r.outerDetId());
}

