#ifndef Validation_MuonGEMHits_GEMSimTrackMatch_H
#define Validation_MuonGEMHits_GEMSimTrackMatch_H

#include "Validation/MuonGEMHits/interface/GEMTrackMatch.h"
#include "Validation/MuonHits/interface/MuonHitMatcher.h"

class GEMSimTrackMatch : public GEMTrackMatch
{
public:
  explicit GEMSimTrackMatch(const edm::ParameterSet& ps);
  ~GEMSimTrackMatch() override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
  void analyze(const edm::Event& e, const edm::EventSetup&) override;

private:
  MonitorElement* track_eta[3];
  MonitorElement* track_phi[3][3];
  MonitorElement* sh_eta[4][3];
  MonitorElement* sh_phi[4][3][3];
  // Local X, Local Y plots are not ready
  //MonitorElement* gem_lx[3][2];
  //MonitorElement* gem_ly[3][2];

  std::unique_ptr<MuonHitMatcher> matcher_;
};

#endif
