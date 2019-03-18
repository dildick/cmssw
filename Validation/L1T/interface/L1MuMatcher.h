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
  void matchGMTToSimTrack(const BXVector<l1t::Muon>&);

  edm::Handle<l1t::EMTFTrackCollection> hl1Tracks_;
  edm::Handle<BXVector<l1t::RegionalMuonCand>> hRegMuonCand_;
  edm::Handle<BXVector<l1t::Muon> > hGMT_;

  edm::EDGetTokenT<l1t::EMTFTrackCollection> emtfTrackToken_;
  edm::EDGetTokenT< BXVector<l1t::RegionalMuonCand> > regionalMuonCandToken_;
  edm::EDGetTokenT< BXVector<l1t::Muon> > gmtToken_;

  std::unique_ptr<CSCStubMatcher> csc_stub_matcher_;
  std::unique_ptr<RPCDigiMatcher> rpc_digi_matcher_;
  std::unique_ptr<GEMDigiMatcher> gem_digi_matcher_;

  float mindREMTFTrack = 10;
  float mindRRegMuCand = 10;
  float mindRGMT = 10;

  int minBXEMTFTrack_, maxBXEMTFTrack_;
  int verboseEMTFTrack_;
  double deltaREMTFTrack_;

  int minBXRegMuCand_, maxBXRegMuCand_;
  int verboseRegMuCand_;
  double deltaRRegMuCand_;

  int minBXGMT_, maxBXGMT_;
  int verboseGMT_;
  double deltaRGMT_;

  std::vector<l1t::EMTFTrack> emtfTracks_;
  std::vector<l1t::RegionalMuonCand> regionalMuonCands_;
  std::vector<l1t::Muon> muons_;

  l1t::EMTFTrack bestEmtfTrack_;
  l1t::RegionalMuonCand bestRegionalMuonCand_;
  l1t::Muon bestMuon_;
};

#endif
