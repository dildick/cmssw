#ifndef Validation_RecoMuon_RecoMuonMatcher_h
#define Validation_RecoMuon_RecoMuonMatcher_h

/**\class RecoMuonMatcher

 Description: Matching of reco muons to SimTrack
*/

#include "Validation/CSCRecHits/interface/CSCRecHitMatcher.h"
#include "Validation/DTRecHits/interface/DTRecHitMatcher.h"
#include "Validation/RPCRecHits/interface/RPCRecHitMatcher.h"
#include "Validation/MuonGEMRecHits/interface/GEMRecHitMatcher.h"
#include "Validation/MuonME0Validation/interface/ME0RecHitMatcher.h"

#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackExtraFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"

class RecoMuonMatcher
{
 public:

  // constructor
  RecoMuonMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC);

  // destructor
  ~RecoMuonMatcher() {}

  // initialize the event
  void init(const edm::Event& e, const edm::EventSetup& eventSetup);

  // do the matching
  void match(const SimTrack& t, const SimVertex& v);

  const reco::TrackExtraCollection& getRecoTrackExtras() const {return recoTrackExtras_;}
  const reco::TrackCollection& getRecoTracks() const {return recoTracks_;}
  const reco::RecoChargedCandidateCollection& getRecoChargedCandidates() const {return recoChargedCandidates_;}

 private:

  void matchRecoTrackExtraToSimTrack(const SimTrack& t, const reco::TrackExtraCollection&);
  void matchRecoTrackToSimTrack(const SimTrack& t, const reco::TrackCollection&);
  void matchRecoChargedCandidateToSimTrack(const SimTrack& t, const reco::RecoChargedCandidateCollection&);

  template<typename T>
  bool areRecoTrackSame(const T&, const T&) const;

  std::unique_ptr<GEMRecHitMatcher> gemRecHitMatcher_;
  std::unique_ptr<ME0RecHitMatcher> me0RecHitMatcher_;
  std::unique_ptr<DTRecHitMatcher>  dtRecHitMatcher_;
  std::unique_ptr<RPCRecHitMatcher> rpcRecHitMatcher_;
  std::unique_ptr<CSCRecHitMatcher> cscRecHitMatcher_;

  int verboseRecoTrackExtra_;
  int verboseRecoTrack_;
  int verboseRecoChargedCandidate_;

  edm::EDGetTokenT<reco::TrackExtraCollection> recoTrackExtraToken_;
  edm::EDGetTokenT<reco::TrackCollection> recoTrackToken_;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> recoChargedCandidateToken_;

  edm::Handle<reco::TrackExtraCollection> recoTrackExtraH_;
  edm::Handle<reco::TrackCollection> recoTrackH_;
  edm::Handle<reco::RecoChargedCandidateCollection> recoChargedCandidateH_;

  reco::TrackExtraCollection recoTrackExtras_;
  reco::TrackCollection recoTracks_;
  reco::RecoChargedCandidateCollection recoChargedCandidates_;

};

#endif
