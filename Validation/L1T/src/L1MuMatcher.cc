#include "Validation/L1T/interface/L1MuMatcher.h"
#include "L1Trigger/L1TMuonEndCap/interface/TrackTools.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/normalizedPhi.h"

#include "TLorentzVector.h"

L1MuMatcher::L1MuMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC)
{
  const auto& tfTrack = iPS.getParameter<edm::ParameterSet>("emtfTrack");
  minBXEMTFTrack_ = tfTrack.getParameter<int>("minBX");
  maxBXEMTFTrack_ = tfTrack.getParameter<int>("maxBX");
  verboseEMTFTrack_ = tfTrack.getParameter<int>("verbose");

  const auto& regionalMuonCand = iPS.getParameter<edm::ParameterSet>("regionalMuonCand");
  minBXRegMuCand_ = regionalMuonCand.getParameter<int>("minBX");
  maxBXRegMuCand_ = regionalMuonCand.getParameter<int>("maxBX");
  verboseRegMuCand_ = regionalMuonCand.getParameter<int>("verbose");
  deltaRRegMuCand_ = regionalMuonCand.getParameter<double>("deltaR");
  deltaPtRelRegMuCand_ = regionalMuonCand.getParameter<double>("deltaPtRel");

  const auto& gmt = iPS.getParameter<edm::ParameterSet>("L1Mu");
  minBXGMT_ = gmt.getParameter<int>("minBX");
  maxBXGMT_ = gmt.getParameter<int>("maxBX");
  verboseGMT_ = gmt.getParameter<int>("verbose");
  deltaRGMT_ = gmt.getParameter<double>("deltaR");
  deltaPtRelGMT_ = gmt.getParameter<double>("deltaPtRel");

  emtfTrackToken_ = iC.consumes<l1t::EMTFTrackCollection>(tfTrack.getParameter<edm::InputTag>("inputTag"));
  regionalMuonCandToken_ = iC.consumes<BXVector<l1t::RegionalMuonCand> >(regionalMuonCand.getParameter<edm::InputTag>("inputTag"));
  gmtToken_ = iC.consumes<BXVector<l1t::Muon> >(gmt.getParameter<edm::InputTag>("inputTag"));

  // initialize the stub matchers
  csc_stub_matcher_.reset(new CSCStubMatcher(iPS, std::move(iC)));
  gem_digi_matcher_.reset(new GEMDigiMatcher(iPS, std::move(iC)));
  rpc_digi_matcher_.reset(new RPCDigiMatcher(iPS, std::move(iC)));
}

void L1MuMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  csc_stub_matcher_->init(iEvent, iSetup);
  gem_digi_matcher_->init(iEvent, iSetup);
  rpc_digi_matcher_->init(iEvent, iSetup);

  iEvent.getByToken(emtfTrackToken_, hl1Tracks_);
  iEvent.getByToken(regionalMuonCandToken_, hRegMuonCand_);
  iEvent.getByToken(gmtToken_, hGMT_);
}

void L1MuMatcher::match(const SimTrack& t, const SimVertex& v)
{
  clear();

  csc_stub_matcher_->match(t,v);
  rpc_digi_matcher_->match(t,v);
  gem_digi_matcher_->match(t,v);

  matchEmtfTrackToSimTrack(*hl1Tracks_.product());
  matchRegionalMuonCandToSimTrack(*hRegMuonCand_.product());
  matchGMTToSimTrack(*hGMT_.product());
}

const std::vector<l1t::EMTFTrack>&
L1MuMatcher::emtfTracks() const
{
  return emtfTracks_;
}

const std::vector<l1t::RegionalMuonCand>&
L1MuMatcher::regionalMuonCands() const
{
  return regionalMuonCands_;
}

const std::vector<l1t::Muon>&
L1MuMatcher::muons() const
{
  return muons_;
}

const l1t::EMTFTrack&
L1MuMatcher::bestEmtfTrack() const
{
  return bestEmtfTrack_;
}

const l1t::RegionalMuonCand&
L1MuMatcher::bestRegionalMuonCand() const
{
  return bestRegionalMuonCand_;
}

const l1t::Muon&
L1MuMatcher::bestMuon() const
{
  return bestMuon_;
}

void
L1MuMatcher::clear()
{
  emtfTracks_.clear();
  regionalMuonCands_.clear();
  muons_.clear();
}

void
L1MuMatcher::matchEmtfTrackToSimTrack(const l1t::EMTFTrackCollection& tracks)
{
  for (const auto& trk : tracks) {
    int nMatchingStubs = 0;
    int nMaxMatchingStubs = 0;
    if (verboseEMTFTrack_)
      std::cout <<"track BX "<< trk.BX()
                <<  " pt "<< trk.Pt()
                <<" eta "<< trk.Eta()
                <<" phi "<< emtf::deg_to_rad(trk.Phi_glob())
                <<" phi_local "<< emtf::deg_to_rad(trk.Phi_loc()) << std::endl;
    for (const auto& stub : trk.Hits()){
      const CSCCorrelatedLCTDigi& csc_stub = stub.CreateCSCCorrelatedLCTDigi();
      const CSCDetId& csc_id = stub.CSC_DetId();
      if (verboseEMTFTrack_) std::cout << "L1 " << csc_id << " " << csc_stub << " " << csc_stub_matcher_->lctsInChamber(csc_id.rawId()).size() << std::endl;
      for (const auto& sim_stub: csc_stub_matcher_->lctsInChamber(csc_id.rawId())){
        if (verboseEMTFTrack_) std::cout << "\tSIM " << csc_id << " " << sim_stub << std::endl;
        if (csc_stub == sim_stub) {
          nMatchingStubs++;
        }
      }
      if (nMatchingStubs>=2) {
        emtfTracks_.push_back(trk);
        if (nMatchingStubs > nMaxMatchingStubs){
          bestEmtfTrack_ = trk;
          nMaxMatchingStubs = nMatchingStubs;
        }
      }
    }
  }
  if (verboseEMTFTrack_){
    std::cout <<"all matched TFTrack size "<< emtfTracks_.size() << std::endl;
    // std::cout <<"best TFTrack ";  bestTrack->print();
  }
}

void L1MuMatcher::matchRegionalMuonCandToSimTrack(const BXVector<l1t::RegionalMuonCand>& regMuCands)
{
  if (emtfTracks_.size()  ==  0) return;

  float maxdPtRel = deltaPtRelRegMuCand_;
  float maxdRRegMuCand = deltaRRegMuCand_;

  for (int bx = regMuCands.getFirstBX(); bx <= regMuCands.getLastBX(); bx++ ){

    if ( bx < minBXRegMuCand_ or bx > maxBXRegMuCand_) continue;

    for (auto cand = regMuCands.begin(bx); cand != regMuCands.end(bx); ++cand ){

      const float cand_pt = cand->hwPt() * 0.5;
      const float cand_eta = cand->hwPt() * 0.010875;
      const float cand_phi = normalizedPhi((((cand->hwPhi() + cand->processor() * 96 + 576 + 24) % 576) / 576.) * 2.0 * 3.1415926);

      if (verboseRegMuCand_)
        std::cout << "candidate regional muon " << cand_pt << " " << cand_eta << " " << cand_phi << std::endl;

      // all matching regional muon cands
      for (const auto& trk : emtfTracks_){

        const float trk_pt = trk.Pt();
        const float trk_eta = trk.Eta();
        const float trk_phi = emtf::deg_to_rad(trk.Phi_glob());

        if (verboseRegMuCand_)
          std::cout << "EMTF track "<< trk_pt  << " " << trk_eta << " " << trk_phi << std::endl;

        // calculate dR
        const float dR = deltaR(trk_eta, trk_phi, cand_eta, cand_phi);

        // calculate delta pT
        const float dPtRel = std::abs(trk_pt - cand_pt)/cand_pt;

        // match by dR and delta pT
        if (dR < deltaRRegMuCand_ and dPtRel < maxdPtRel){
          regionalMuonCands_.push_back(*cand);
        }
      }

      const float bestTrack_eta = bestEmtfTrack_.Eta();
      const float bestTrack_phi = emtf::deg_to_rad(bestEmtfTrack_.Phi_glob());

      const float dR = deltaR(bestTrack_eta, bestTrack_phi, cand_eta, cand_phi);

      // find the best matching
      if (dR < maxdRRegMuCand){
        maxdRRegMuCand = dR;
        bestRegionalMuonCand_ = *cand;
      }
    } // end loop RegionalMuonCand
  } // end loop BX
}

void L1MuMatcher::matchGMTToSimTrack(const BXVector<l1t::Muon>& gmtCands)
{
  /*
  if (emtfTracks_.size()  ==  0) return;
  float mindPtRel = 0.5;
  maxdRGMT = deltaRGMT_;
  for (int bx = gmtCands.getFirstBX(); bx <= gmtCands.getLastBX(); bx++ ){
    if ( bx < minBXGMT_ or bx > maxBXGMT_) continue;
    for (auto cand = gmtCands.begin(bx); cand != gmtCands.end(bx); ++cand ){
      TFCand *L1Mu = new TFCand(&(*cand));
      L1Mu->setBx(bx);
      float pt = L1Mu->pt();
      float phi = L1Mu->phi() ;
      float eta = L1Mu->eta();
      for (const auto& trk : emtfTracks_){
        float dR = deltaR(trk->eta(), trk->phi(), eta, phi);
        float dPtRel = std::fabs(trk->pt() - pt)/pt;
        if (dR < deltaRGMT_ and dPtRel < mindPtRel){
          L1Mu->setDR( dR );
          L1Mu->setMatchedTFTrack( trk );
          gmts_.push_back(L1Mu);
        }
      }
      if (verboseGMT_)
        L1Mu->print();
    }
  }
  for (const auto& cand : gmts_){
    float phi = cand->phi();
    float eta = cand->eta();
    float dR = deltaR(bestTrack->eta(), bestTrack->phi(), eta, phi);
    if (dR < maxdRGMT){
      maxdRGMT = dR;
      bestGMT = cand;
      if (verboseGMT_){
        std::cout <<"bestGMT "; bestGMT->print();
      }
    }
  }
  */
}

