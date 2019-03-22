#include "Validation/L1T/interface/L1MuMatcher.h"
#include "L1Trigger/L1TMuonEndCap/interface/TrackTools.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/normalizedPhi.h"

#include "TLorentzVector.h"

L1MuMatcher::L1MuMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC)
{
  const auto& emtfTrack = iPS.getParameter<edm::ParameterSet>("emtfTrack");
  minBXEMTFTrack_ = emtfTrack.getParameter<int>("minBX");
  maxBXEMTFTrack_ = emtfTrack.getParameter<int>("maxBX");
  verboseEMTFTrack_ = emtfTrack.getParameter<int>("verbose");

  const auto& bmtfCand = iPS.getParameter<edm::ParameterSet>("bmtfCand");
  minBXBmtfCand_ = bmtfCand.getParameter<int>("minBX");
  maxBXBmtfCand_ = bmtfCand.getParameter<int>("maxBX");
  verboseBmtfCand_ = bmtfCand.getParameter<int>("verbose");
  deltaRBmtfCand_ = bmtfCand.getParameter<double>("deltaR");
  deltaPtRelBmtfCand_ = bmtfCand.getParameter<double>("deltaPtRel");

  const auto& omtfCand = iPS.getParameter<edm::ParameterSet>("omtfCand");
  minBXOmtfCand_ = omtfCand.getParameter<int>("minBX");
  maxBXOmtfCand_ = omtfCand.getParameter<int>("maxBX");
  verboseOmtfCand_ = omtfCand.getParameter<int>("verbose");
  deltaROmtfCand_ = omtfCand.getParameter<double>("deltaR");
  deltaPtRelOmtfCand_ = omtfCand.getParameter<double>("deltaPtRel");

  const auto& emtfCand = iPS.getParameter<edm::ParameterSet>("emtfCand");
  minBXEmtfCand_ = emtfCand.getParameter<int>("minBX");
  maxBXEmtfCand_ = emtfCand.getParameter<int>("maxBX");
  verboseEmtfCand_ = emtfCand.getParameter<int>("verbose");
  deltaREmtfCand_ = emtfCand.getParameter<double>("deltaR");
  deltaPtRelEmtfCand_ = emtfCand.getParameter<double>("deltaPtRel");

  const auto& muon = iPS.getParameter<edm::ParameterSet>("gmt");
  minBXMuon_ = muon.getParameter<int>("minBX");
  maxBXMuon_ = muon.getParameter<int>("maxBX");
  verboseMuon_ = muon.getParameter<int>("verbose");
  deltaRMuon_ = muon.getParameter<double>("deltaR");
  deltaPtRelMuon_ = muon.getParameter<double>("deltaPtRel");

  emtfTrackToken_ = iC.consumes<l1t::EMTFTrackCollection>(emtfTrack.getParameter<edm::InputTag>("inputTag"));
  bmtfCandToken_ = iC.consumes<BXVector<l1t::RegionalMuonCand> >(bmtfCand.getParameter<edm::InputTag>("inputTag"));
  omtfCandToken_ = iC.consumes<BXVector<l1t::RegionalMuonCand> >(omtfCand.getParameter<edm::InputTag>("inputTag"));
  emtfCandToken_ = iC.consumes<BXVector<l1t::RegionalMuonCand> >(emtfCand.getParameter<edm::InputTag>("inputTag"));
  muonToken_ = iC.consumes<BXVector<l1t::Muon> >(muon.getParameter<edm::InputTag>("inputTag"));

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

  iEvent.getByToken(emtfTrackToken_, hEmtfTracks_);
  iEvent.getByToken(emtfCandToken_, hEmtfCand_);
  iEvent.getByToken(muonToken_, hMuon_);
}

void L1MuMatcher::match(const SimTrack& t, const SimVertex& v)
{
  clear();

  csc_stub_matcher_->match(t,v);
  rpc_digi_matcher_->match(t,v);
  gem_digi_matcher_->match(t,v);

  matchEmtfTrackToSimTrack(*hEmtfTracks_.product());
  matchEmtfCandToSimTrack(*hEmtfCand_.product());
  matchMuonToSimTrack(*hMuon_.product());
}

const std::vector<l1t::EMTFTrack>&
L1MuMatcher::emtfTracks() const
{
  return emtfTracks_;
}

const std::vector<l1t::RegionalMuonCand>&
L1MuMatcher::bmtfCands() const
{
  return bmtfCands_;
}

const std::vector<l1t::RegionalMuonCand>&
L1MuMatcher::omtfCands() const
{
  return omtfCands_;
}

const std::vector<l1t::RegionalMuonCand>&
L1MuMatcher::emtfCands() const
{
  return emtfCands_;
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
L1MuMatcher::bestBmtfCand() const
{
  return bestBmtfCand_;
}

const l1t::RegionalMuonCand&
L1MuMatcher::bestOmtfCand() const
{
  return bestOmtfCand_;
}

const l1t::RegionalMuonCand&
L1MuMatcher::bestEmtfCand() const
{
  return bestEmtfCand_;
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
  bmtfCands_.clear();
  omtfCands_.clear();
  emtfCands_.clear();
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

void L1MuMatcher::matchEmtfCandToSimTrack(const BXVector<l1t::RegionalMuonCand>& regMuCands)
{
  if (emtfTracks_.size()  ==  0) return;

  float maxdPtRel = deltaPtRelEmtfCand_;
  float maxdREmtfCand = deltaREmtfCand_;

  for (int bx = regMuCands.getFirstBX(); bx <= regMuCands.getLastBX(); bx++ ){

    // out of time muon
    if ( bx < minBXEmtfCand_ or bx > maxBXEmtfCand_) continue;

    for (auto cand = regMuCands.begin(bx); cand != regMuCands.end(bx); ++cand ){

      const float cand_pt = cand->hwPt() * 0.5;
      const float cand_eta = cand->hwPt() * 0.010875;
      const float cand_phi = normalizedPhi((((cand->hwPhi() + cand->processor() * 96 + 576 + 24) % 576) / 576.) * 2.0 * 3.1415926);

      if (verboseEmtfCand_)
        std::cout << "candidate regional muon " << cand_pt << " " << cand_eta << " " << cand_phi << std::endl;

      // all matching regional muon cands
      for (const auto& trk : emtfTracks_){

        const float trk_pt = trk.Pt();
        const float trk_eta = trk.Eta();
        const float trk_phi = emtf::deg_to_rad(trk.Phi_glob());

        if (verboseEmtfCand_)
          std::cout << "EMTF track "<< trk_pt  << " " << trk_eta << " " << trk_phi << std::endl;

        // calculate dR
        const float dR = deltaR(trk_eta, trk_phi, cand_eta, cand_phi);

        // calculate delta pT
        const float dPtRel = std::abs(trk_pt - cand_pt)/cand_pt;

        // match by dR and delta pT
        if (dR < deltaREmtfCand_ and dPtRel < maxdPtRel){
          emtfCands_.push_back(*cand);
        }
      }

      const float bestTrack_eta = bestEmtfTrack_.Eta();
      const float bestTrack_phi = emtf::deg_to_rad(bestEmtfTrack_.Phi_glob());

      const float dR = deltaR(bestTrack_eta, bestTrack_phi, cand_eta, cand_phi);

      // find the best matching
      if (dR < maxdREmtfCand){
        maxdREmtfCand = dR;
        bestEmtfCand_ = *cand;
      }
    } // end loop RegionalMuonCand
  } // end loop BX
}

void L1MuMatcher::matchMuonToSimTrack(const BXVector<l1t::Muon>& muons)
{
  if (emtfCands_.size()  ==  0) return;
  float maxdPtRel = deltaPtRelMuon_;
  float maxdRMuon = deltaRMuon_;

  for (int bx = muons.getFirstBX(); bx <= muons.getLastBX(); bx++ ){

    // out of time muon
    if ( bx < minBXMuon_ or bx > maxBXMuon_) continue;

    for (auto muon = muons.begin(bx); muon != muons.end(bx); ++muon ){

      const float muon_pt = muon->hwPt() * 0.5;
      const float muon_eta = muon->hwPt() * 0.010875;
      const float muon_phi = normalizedPhi(muon->hwPhi() * 0.010809);

      if (verboseMuon_)
        std::cout << "Muon " << muon_pt << " " << muon_eta << " " << muon_phi << std::endl;

      // all matching regional muon cands
      for (const auto& cand : emtfCands_) {

        const float cand_pt = cand.hwPt() * 0.5;
        const float cand_eta = cand.hwPt() * 0.010875;
        const float cand_phi = normalizedPhi((((cand.hwPhi() + cand.processor() * 96 + 576 + 24) % 576) / 576.) * 2.0 * 3.1415926);

        if (verboseMuon_)
          std::cout << "RegionalMuonCand "<< cand_pt  << " " << cand_eta << " " << cand_phi << std::endl;

        // calculate dR
        const float dR = deltaR(cand_eta, cand_phi, muon_eta, muon_phi);

        // calculate delta pT
        const float dPtRel = std::abs(cand_pt - muon_pt)/muon_pt;

        // match by dR and delta pT
        if (dR < deltaRMuon_ and dPtRel < maxdPtRel){
          muons_.push_back(*muon);
        }
      }

      const float bestRegionalMuonCand_eta = bestEmtfCand_.hwPt() * 0.010875;
      const float bestRegionalMuonCand_phi(normalizedPhi((((bestEmtfCand_.hwPhi() +
                                                            bestEmtfCand_.processor() * 96 + 576 + 24) % 576) / 576.) * 2.0 * 3.1415926));

      const float dR = deltaR(bestRegionalMuonCand_eta, bestRegionalMuonCand_phi, muon_eta, muon_phi);

      // find the best matching
      if (dR < maxdRMuon){
        maxdRMuon = dR;
        bestMuon_ = *muon;
      }
    } // end loop RegionalMuonCand
  } // end loop BX
}

