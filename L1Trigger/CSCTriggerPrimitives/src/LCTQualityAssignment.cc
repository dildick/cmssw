#include "L1Trigger/CSCTriggerPrimitives/interface/LCTQualityAssignment.h"
#include "L1Trigger/CSCTriggerPrimitives/interface/GEMInternalCluster.h"
#include "DataFormats/CSCDigi/interface/CSCConstants.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigi.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigi.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

LCTQualityAssignment::LCTQualityAssignment(unsigned station) : station_(station) {}

unsigned LCTQualityAssignment::findQuality(const CSCALCTDigi& aLCT, const CSCCLCTDigi& cLCT, bool runCCLUT) const {
  if (runCCLUT)
    return findQualityRun3(aLCT, cLCT);
  else
    return findQualityRun2(aLCT, cLCT);
}

// 4-bit LCT quality number.
unsigned LCTQualityAssignment::findQualityRun2(const CSCALCTDigi& aLCT, const CSCCLCTDigi& cLCT) const {
  LCT_QualityRun2 qual = LCT_QualityRun2::INVALID;

  // Either ALCT or CLCT is invalid
  if (!(aLCT.isValid()) || !(cLCT.isValid())) {
    // No CLCT
    if (aLCT.isValid() && !(cLCT.isValid()))
      qual = LCT_QualityRun2::NO_CLCT;

    // No ALCT
    else if (!(aLCT.isValid()) && cLCT.isValid())
      qual = LCT_QualityRun2::NO_ALCT;

    // No ALCT and no CLCT
    else
      qual = LCT_QualityRun2::INVALID;
  }
  // Both ALCT and CLCT are valid
  else {
    const int pattern(cLCT.getPattern());

    // Layer-trigger in CLCT
    if (pattern == 1)
      qual = LCT_QualityRun2::CLCT_LAYER_TRIGGER;

    // Multi-layer pattern in CLCT
    else {
      // ALCT quality is the number of layers hit minus 3.
      const bool a4(aLCT.getQuality() >= 1);

      // CLCT quality is the number of layers hit.
      const bool c4(cLCT.getQuality() >= 4);

      // quality = 4; "reserved for low-quality muons in future"

      // marginal anode and cathode
      if (!a4 && !c4)
        qual = LCT_QualityRun2::MARGINAL_ANODE_CATHODE;

      // HQ anode, but marginal cathode
      else if (a4 && !c4)
        qual = LCT_QualityRun2::HQ_ANODE_MARGINAL_CATHODE;

      // HQ cathode, but marginal anode
      else if (!a4 && c4)
        qual = LCT_QualityRun2::HQ_CATHODE_MARGINAL_ANODE;

      // HQ muon, but accelerator ALCT
      else if (a4 && c4) {
        if (aLCT.getAccelerator())
          qual = LCT_QualityRun2::HQ_ACCEL_ALCT;

        else {
          // quality =  9; "reserved for HQ muons with future patterns
          // quality = 10; "reserved for HQ muons with future patterns

          // High quality muons are determined by their CLCT pattern
          if (pattern == 2 || pattern == 3)
            qual = LCT_QualityRun2::HQ_PATTERN_2_3;

          else if (pattern == 4 || pattern == 5)
            qual = LCT_QualityRun2::HQ_PATTERN_4_5;

          else if (pattern == 6 || pattern == 7)
            qual = LCT_QualityRun2::HQ_PATTERN_6_7;

          else if (pattern == 8 || pattern == 9)
            qual = LCT_QualityRun2::HQ_PATTERN_8_9;

          else if (pattern == 10)
            qual = LCT_QualityRun2::HQ_PATTERN_10;

          else {
            edm::LogWarning("LCTQualityAssignment") << "findQualityRun2: Unexpected CLCT pattern id = " << pattern;
            qual = LCT_QualityRun2::INVALID;
          }
        }
      }
    }
  }
  return static_cast<unsigned>(qual);
}

// 2-bit LCT quality number for Run-3
unsigned LCTQualityAssignment::findQualityRun3(const CSCALCTDigi& aLCT, const CSCCLCTDigi& cLCT) const {
  LCT_QualityRun3 qual = LCT_QualityRun3::INVALID;

  // Run-3 definition
  if (!(aLCT.isValid()) and !(cLCT.isValid())) {
    qual = LCT_QualityRun3::INVALID;
  }
  // use number of layers on each as indicator
  else {
    const bool a4 = (aLCT.getQuality() == 1);
    const bool a5 = (aLCT.getQuality() == 2);
    const bool a6 = (aLCT.getQuality() == 3);

    const bool c4 = (cLCT.getQuality() == 4);
    const bool c5 = (cLCT.getQuality() == 5);
    const bool c6 = (cLCT.getQuality() == 6);
    if (a6 or c6)
      qual = LCT_QualityRun3::HighQ;
    else if (a5 or c5)
      qual = LCT_QualityRun3::MedQ;
    else if (a4 or c4)
      qual = LCT_QualityRun3::LowQ;
  }
  return static_cast<unsigned>(qual);
}

unsigned LCTQualityAssignment::findQualityGEMv1(const CSCCLCTDigi& cLCT, const GEMInternalCluster& cl) const {
  LCT_QualityRun2 qual = LCT_QualityRun2::INVALID;

  if (!cl.isCoincidence() or !cLCT.isValid())
    return static_cast<unsigned>(qual);

  const int pattern(cLCT.getPattern());

  // High quality muons are determined by their CLCT pattern
  if (pattern == 2 || pattern == 3)
    qual = LCT_QualityRun2::HQ_PATTERN_2_3;

  else if (pattern == 4 || pattern == 5)
    qual = LCT_QualityRun2::HQ_PATTERN_4_5;

  else if (pattern == 6 || pattern == 7)
    qual = LCT_QualityRun2::HQ_PATTERN_6_7;

  else if (pattern == 8 || pattern == 9)
    qual = LCT_QualityRun2::HQ_PATTERN_8_9;

  else if (pattern == 10)
    qual = LCT_QualityRun2::HQ_PATTERN_10;

  else {
    edm::LogWarning("CSCGEMMotherboard") << "findQualityGEMv1: Unexpected CLCT pattern id = " << pattern;
    qual = LCT_QualityRun2::INVALID;
  }

  return static_cast<unsigned>(qual);
}

unsigned LCTQualityAssignment::findQualityGEMv1(const CSCALCTDigi& aLCT,
                                                const CSCCLCTDigi& cLCT,
                                                const GEMInternalCluster& cl) const {
  LCT_QualityRun2 qual = LCT_QualityRun2::INVALID;

  int gemlayers = 0;
  if (cl.isValid())
    gemlayers = 1;
  if (cl.isCoincidence())
    gemlayers = 2;

  // Either ALCT or CLCT is invalid
  if (!(aLCT.isValid()) || !(cLCT.isValid())) {
    // No CLCT
    if (aLCT.isValid() && !(cLCT.isValid()))
      qual = LCT_QualityRun2::NO_CLCT;

    // No ALCT
    else if (!(aLCT.isValid()) && cLCT.isValid())
      qual = LCT_QualityRun2::NO_ALCT;

    // No ALCT and no CLCT
    else
      qual = LCT_QualityRun2::INVALID;
  }
  // Both ALCT and CLCT are valid
  else {
    const int pattern(cLCT.getPattern());

    // Layer-trigger in CLCT
    if (pattern == 1)
      qual = LCT_QualityRun2::CLCT_LAYER_TRIGGER;

    // Multi-layer pattern in CLCT
    else {
      // ALCT quality is the number of layers hit minus 3.
      bool a4 = false;

      // Case of ME11 with GEMs: require 4 layers for ALCT
      if (station_ == 1)
        a4 = aLCT.getQuality() >= 1;

      // Case of ME21 with GEMs: require 4 layers for ALCT+GEM
      if (station_ == 2)
        a4 = aLCT.getQuality() + gemlayers >= 1;

      // CLCT quality is the number of layers hit.
      const bool c4((cLCT.getQuality() >= 4) or (cLCT.getQuality() >= 3 and gemlayers >= 1));

      // quality = 4; "reserved for low-quality muons in future"

      // marginal anode and cathode
      if (!a4 && !c4)
        qual = LCT_QualityRun2::MARGINAL_ANODE_CATHODE;

      // HQ anode, but marginal cathode
      else if (a4 && !c4)
        qual = LCT_QualityRun2::HQ_ANODE_MARGINAL_CATHODE;

      // HQ cathode, but marginal anode
      else if (!a4 && c4)
        qual = LCT_QualityRun2::HQ_CATHODE_MARGINAL_ANODE;

      // HQ muon, but accelerator ALCT
      else if (a4 && c4) {
        if (aLCT.getAccelerator())
          qual = LCT_QualityRun2::HQ_ACCEL_ALCT;

        else {
          // quality =  9; "reserved for HQ muons with future patterns
          // quality = 10; "reserved for HQ muons with future patterns

          // High quality muons are determined by their CLCT pattern
          if (pattern == 2 || pattern == 3)
            qual = LCT_QualityRun2::HQ_PATTERN_2_3;

          else if (pattern == 4 || pattern == 5)
            qual = LCT_QualityRun2::HQ_PATTERN_4_5;

          else if (pattern == 6 || pattern == 7)
            qual = LCT_QualityRun2::HQ_PATTERN_6_7;

          else if (pattern == 8 || pattern == 9)
            qual = LCT_QualityRun2::HQ_PATTERN_8_9;

          else if (pattern == 10)
            qual = LCT_QualityRun2::HQ_PATTERN_10;

          else {
            edm::LogWarning("CSCGEMMotherboard") << "findQualityGEMv1: Unexpected CLCT pattern id = " << pattern;
            qual = LCT_QualityRun2::INVALID;
          }
        }
      }
    }
  }
  return static_cast<unsigned>(qual);
}

unsigned LCTQualityAssignment::findQualityGEMv2(const CSCALCTDigi& aLCT,
                                                const CSCCLCTDigi& cLCT,
                                                const GEMInternalCluster& cl,
                                                bool assignGEMCSCBending) const {
  LCT_QualityRun3GEM qual = LCT_QualityRun3GEM::INVALID;

  const bool aValid(aLCT.isValid());
  const bool cValid(cLCT.isValid());
  const bool gValid(cl.isValid());
  const bool ggValid(cl.isValid() and cl.isCoincidence());

  // ALCT-CLCT-2GEM
  if (aValid and cValid and ggValid) {
    if (assignGEMCSCBending)
      qual = LCT_QualityRun3GEM::ALCT_CLCT_2GEM_GEMCSCBend;
    else
      qual = LCT_QualityRun3GEM::ALCT_CLCT_2GEM_CSCBend;
  }

  // ALCT-CLCT-1GEM
  else if (aValid and cValid and gValid) {
    if (assignGEMCSCBending)
      qual = LCT_QualityRun3GEM::ALCT_CLCT_1GEM_GEMCSCBend;
    else
      qual = LCT_QualityRun3GEM::ALCT_CLCT_1GEM_CSCBend;
  }

  // ALCT-CLCT
  else if (aValid and cValid) {
    qual = LCT_QualityRun3GEM::ALCT_CLCT;
  }

  // CLCT-2GEM
  else if (cValid and ggValid) {
    qual = LCT_QualityRun3GEM::CLCT_2GEM;
  }

  // ALCT-2GEM
  else if (aValid and ggValid) {
    qual = LCT_QualityRun3GEM::ALCT_2GEM;
  }

  // at this point we have exhausted all possibilities
  // only remaing option is an invalid LCT
  else
    qual = LCT_QualityRun3GEM::INVALID;

  return static_cast<unsigned>(qual);
}
