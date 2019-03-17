#ifndef Validation_MuonCSCDigis_CSCStubMatcher_h
#define Validation_MuonCSCDigis_CSCStubMatcher_h

/**\class CSCStubMatcher

 Description: Matching of CSC L1 trigger stubs to SimTrack

*/

#include "Validation/MuonCSCDigis/interface/CSCDigiMatcher.h"
#include "Validation/MuonGEMDigis/interface/GEMDigiMatcher.h"

#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"

typedef std::vector<CSCALCTDigi> CSCALCTDigiContainer;
typedef std::vector<CSCCLCTDigi> CSCCLCTDigiContainer;
typedef std::vector<CSCCorrelatedLCTDigi> CSCCorrelatedLCTDigiContainer;

class CSCStubMatcher
{
public:

  CSCStubMatcher(const edm::Event&, const edm::EventSetup&,
                 edm::ParameterSet const& iPS, edm::ConsumesCollector && iC,
                 const CSCDigiMatcher& dg, const GEMDigiMatcher& gem_dg);

  ~CSCStubMatcher();

  /// crossed chamber detIds with not necessarily matching stubs
  std::set<unsigned int> chamberIdsAllCLCT(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsAllALCT(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsAllLCT(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsAllMPLCT(int csc_type = CSC_ALL) const;

  /// chamber detIds with matching stubs
  std::set<unsigned int> chamberIdsCLCT(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsALCT(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsLCT(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsMPLCT(int csc_type = CSC_ALL) const;

  /// all stubs (not necessarily matching) from a particular crossed chamber
  const CSCCLCTDigiContainer& allCLCTsInChamber(unsigned int) const;
  const CSCALCTDigiContainer& allALCTsInChamber(unsigned int) const;
  const CSCCorrelatedLCTDigiContainer& allLCTsInChamber(unsigned int) const;
  const CSCCorrelatedLCTDigiContainer& allMPLCTsInChamber(unsigned int) const;

  /// all matching from a particular crossed chamber
  const CSCCLCTDigiContainer& clctsInChamber(unsigned int) const;
  const CSCALCTDigiContainer& alctsInChamber(unsigned int) const;
  const CSCCorrelatedLCTDigiContainer& lctsInChamber(unsigned int) const;
  const CSCCorrelatedLCTDigiContainer& mplctsInChamber(unsigned int) const;

  /// all matching lcts
  std::map<unsigned int, CSCCLCTDigiContainer> alcts() const { return chamber_to_alcts_; }
  std::map<unsigned int, CSCCLCTDigiContainer> clcts() const { return chamber_to_clcts_; }
  std::map<unsigned int, CSCCorrelatedLCTDigiContainer> lcts() const { return chamber_to_lcts_; }
  std::map<unsigned int, CSCCorrelatedLCTDigiContainer> mplcts() const { return chamber_to_mplcts_; }

  /// best matching from a particular crossed chamber
  CSCCLCTDigi bestClctInChamber(unsigned int) const;
  CSCALCTDigi bestAlctInChamber(unsigned int) const;
  CSCCorrelatedLCTDigi bestLctInChamber(unsigned int) const;
  CSCCorrelatedLCTDigi bestMplctInChamber(unsigned int) const;

  //z position of  certain layer
  float zpositionOfLayer(unsigned int detid, int layer) const;

  /// How many CSC chambers with matching stubs of some minimal quality did this SimTrack hit?
  int nChambersWithCLCT(int min_quality = 0) const;
  int nChambersWithALCT(int min_quality = 0) const;
  int nChambersWithLCT(int min_quality = 0) const;
  int nChambersWithMPLCT(int min_quality = 0) const;

  bool lctInChamber(const CSCDetId& id, const CSCCorrelatedLCTDigi& lct) const;

  // get the position of an LCT in global coordinates
  GlobalPoint getGlobalPosition(unsigned int rawId, const CSCCorrelatedLCTDigi& lct) const;

  // get the bending angle from the pattern number and bending bit
  float getAverageBendingLCT(unsigned int rawId, const CSCCorrelatedLCTDigi& lct) const;

private:

  void matchCLCTsToSimTrack(const CSCCLCTDigiCollection&);
  void matchALCTsToSimTrack(const CSCALCTDigiCollection&);
  void matchLCTsToSimTrack(const CSCCorrelatedLCTDigiCollection&);
  void matchMPLCTsToSimTrack(const CSCCorrelatedLCTDigiCollection&);

  std::unique_ptr<CSCDigiMatcher> csc_digi_matcher_;
  std::unique_ptr<GEMDigiMatcher> gem_digi_matcher_;
  std::unique_ptr<CSCGeometry> csc_geometry_;

  // all stubs (not necessarily matching) in crossed chambers with digis
  std::map<unsigned int, CSCCLCTDigiContainer> chamber_to_clcts_all_;
  std::map<unsigned int, CSCALCTDigiContainer> chamber_to_alcts_all_;
  std::map<unsigned int, CSCCorrelatedLCTDigiContainer> chamber_to_lcts_all_;
  std::map<unsigned int, CSCCorrelatedLCTDigiContainer> chamber_to_mplcts_all_;

  // all matching stubs in crossed chambers with digis
  std::map<unsigned int, CSCCLCTDigiContainer> chamber_to_clcts_;
  std::map<unsigned int, CSCALCTDigiContainer> chamber_to_alcts_;
  std::map<unsigned int, CSCCorrelatedLCTDigiContainer> chamber_to_lcts_;
  std::map<unsigned int, CSCCorrelatedLCTDigiContainer> chamber_to_mplcts_;

  template<class D>
  std::set<unsigned int> selectDetIds(D &, int) const;

  bool hsFromSimHitMean_;

  int minNHitsChamber_;
  int minNHitsChamberALCT_;
  int minNHitsChamberCLCT_;
  int minNHitsChamberLCT_;
  int minNHitsChamberMPLCT_;

  bool verboseALCT_;
  bool verboseCLCT_;
  bool verboseLCT_;
  bool verboseMPLCT_;

  int minBXCLCT_, maxBXCLCT_;
  int minBXALCT_, maxBXALCT_;
  int minBXLCT_, maxBXLCT_;
  int minBXMPLCT_, maxBXMPLCT_;

  CSCCLCTDigiContainer no_clcts_;
  CSCALCTDigiContainer no_alcts_;
  CSCCorrelatedLCTDigiContainer no_lcts_;
  CSCCorrelatedLCTDigiContainer no_mplcts_;
};


template<class D>
std::set<unsigned int>
CSCStubMatcher::selectDetIds(D &digis, int csc_type) const
{
  std::set<unsigned int> result;
  for (auto& p: digis)
  {
    auto id = p.first;
    if (csc_type > 0)
    {
      CSCDetId detId(id);
      if (gemvalidation::toCSCType(detId.station(), detId.ring()) != csc_type) continue;
    }
    result.insert(p.first);
  }
  return result;
}

#endif
