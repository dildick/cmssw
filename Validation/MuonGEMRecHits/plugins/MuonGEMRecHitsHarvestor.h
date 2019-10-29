#ifndef Validation_MuonGEMRecHits_MuonGEMDigisHarvestor_H
#define Validation_MuonGEMRecHits_MuonGEMDigisHarvestor_H

#include "FWCore/Framework/interface/ESHandle.h"
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "SimMuon/MCTruth/interface/PSimHitMap.h"

#include "Validation/MuonGEMRecHits/plugins/GEMRecHitValidation.h"
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>
#include <TProfile.h>

class MuonGEMRecHitsHarvestor : public DQMEDHarvester {
public:
  /// constructor
  explicit MuonGEMRecHitsHarvestor(const edm::ParameterSet&);
  /// destructor
  ~MuonGEMRecHitsHarvestor() override;

  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override;
  void ProcessBooking(
      DQMStore::IBooker&, DQMStore::IGetter&, const char* label, TString suffix, TH1F* track_hist, TH1F* sh_hist);
  TProfile* ComputeEff(TH1F* num, TH1F* denum);

private:
  std::string dbe_path_, outputFile_;
};
#endif
