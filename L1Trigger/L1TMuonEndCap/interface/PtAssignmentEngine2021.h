#ifndef L1TMuonEndCap_PtAssignmentEngine2021_h
#define L1TMuonEndCap_PtAssignmentEngine2021_h

#include "L1Trigger/L1TMuonEndCap/interface/PtAssignmentEngine.h"
#include "L1Trigger/L1TMuonEndCap/interface/PtAssignmentEngineAux2021.h"

class PtAssignmentEngine2021 : public PtAssignmentEngine {
public:
  explicit PtAssignmentEngine2021() : PtAssignmentEngine() {}
  ~PtAssignmentEngine2021() override {}

  const PtAssignmentEngineAux2021& aux() const;

  float scale_pt(const float pt, const int mode = 15) const override;
  float unscale_pt(const float pt, const int mode = 15) const override;
  address_t calculate_address(const EMTFTrack& track) const override;
  float calculate_pt_xml(const address_t& address) const override;
  float calculate_pt_xml(const EMTFTrack& track) const override;

private:
};

#endif
