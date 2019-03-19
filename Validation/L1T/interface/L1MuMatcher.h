#ifndef Validation_L1T_L1MuMatcher_h
#define Validation_L1T_L1MuMatcher_h

/**\class L1MuMatcher

 Description: Matching of simtracks to L1Mu through stubs and digis
*/

#include "Validation/MuonCSCDigis/interface/CSCStubMatcher.h"
#include "Validation/MuonRPCDigis/interface/RPCDigiMatcher.h"
#include "Validation/MuonGEMDigis/interface/GEMDigiMatcher.h"

#include "DataFormats/L1TMuon/interface/EMTFTrack.h"
#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"
#include "DataFormats/L1Trigger/interface/Muon.h"

class L1MuMatcher
{
 public:
  /// constructor
  L1MuMatcher(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC);

  /// destructor
  ~L1MuMatcher() {}

  /// initialize the event
  void init(const edm::Event& iEvent, const edm::EventSetup& iSetup);

  /// do the matching
  void match(const SimTrack& t, const SimVertex& v);

  /// all matching
  const std::vector<l1t::EMTFTrack>& emtfTracks() const;
  const std::vector<l1t::RegionalMuonCand>& regionalMuonCands() const;
  const std::vector<l1t::Muon>& muons() const;

  /// best matching
  const l1t::EMTFTrack& bestEmtfTrack() const;
  const l1t::RegionalMuonCand& bestRegionalMuonCand() const;
  const l1t::Muon& bestMuon() const;

 private:

  void clear();

  void matchEmtfTrackToSimTrack(const l1t::EMTFTrackCollection&);
  void matchRegionalMuonCandToSimTrack(const BXVector<l1t::RegionalMuonCand>&);
  void matchMuonToSimTrack(const BXVector<l1t::Muon>&);

  edm::Handle<l1t::EMTFTrackCollection> hl1Tracks_;
  edm::Handle<BXVector<l1t::RegionalMuonCand>> hRegMuonCand_;
  edm::Handle<BXVector<l1t::Muon> > hMuon_;

  edm::EDGetTokenT<l1t::EMTFTrackCollection> emtfTrackToken_;
  edm::EDGetTokenT< BXVector<l1t::RegionalMuonCand> > regionalMuonCandToken_;
  edm::EDGetTokenT< BXVector<l1t::Muon> > muonToken_;

  std::unique_ptr<CSCStubMatcher> csc_stub_matcher_;
  std::unique_ptr<RPCDigiMatcher> rpc_digi_matcher_;
  std::unique_ptr<GEMDigiMatcher> gem_digi_matcher_;

  // eta and phi are the same for
  // EMTF, RegionalMuonCand and L1Mu
  float maxdRRegMuCand = 0.001;
  float maxdRMuon = 0.001;

  int minBXEMTFTrack_, maxBXEMTFTrack_;
  int verboseEMTFTrack_;

  int minBXRegMuCand_, maxBXRegMuCand_;
  int verboseRegMuCand_;
  double deltaRRegMuCand_;
  double deltaPtRelRegMuCand_;

  int minBXMuon_, maxBXMuon_;
  int verboseMuon_;
  double deltaRMuon_;
  double deltaPtRelMuon_;

  std::vector<l1t::EMTFTrack> emtfTracks_;
  std::vector<l1t::RegionalMuonCand> regionalMuonCands_;
  std::vector<l1t::Muon> muons_;

  l1t::EMTFTrack bestEmtfTrack_;
  l1t::RegionalMuonCand bestRegionalMuonCand_;
  l1t::Muon bestMuon_;
};

#endif
