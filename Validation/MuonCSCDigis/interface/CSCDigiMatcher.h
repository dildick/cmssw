#ifndef Validation_MuonCSCDigis_CSCDigiMatcher_h
#define Validation_MuonCSCDigis_CSCDigiMatcher_h

/**\class CSCDigiMatcher

   Description: Matching of Digis to SimTrack in CSC
*/

#include "Validation/MuonHits/interface/SimHitMatcher.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"

typedef std::vector<CSCComparatorDigi> CSCComparatorDigiContainer;
typedef std::vector<CSCStripDigi> CSCStripDigiContainer;
typedef std::vector<CSCWireDigi> CSCWireDigiContainer;

typedef std::vector<std::pair<unsigned int, CSCComparatorDigi> > CSCComparatorDigiDetIdContainer;
typedef std::vector<std::pair<unsigned int, CSCStripDigi> > CSCStripDigiDetIdContainer;
typedef std::vector<std::pair<unsigned int, CSCWireDigi> > CSCWireDigiDetIdContainer;

class SimHitMatcher;

class CSCDigiMatcher
{
public:

  CSCDigiMatcher(const edm::Event&, const edm::EventSetup&,
                 edm::ParameterSet const& iPS, edm::ConsumesCollector && iC,
                 const SimHitMatcher&);

  ~CSCDigiMatcher();

  /// layer detIds with digis
  std::set<unsigned int> detIdsComparator(int csc_type = CSC_ALL) const;
  std::set<unsigned int> detIdsStrip(int csc_type = CSC_ALL) const;
  std::set<unsigned int> detIdsWire(int csc_type = CSC_ALL) const;

  /// chamber detIds with digis
  std::set<unsigned int> chamberIdsComparator(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsStrip(int csc_type = CSC_ALL) const;
  std::set<unsigned int> chamberIdsWire(int csc_type = CSC_ALL) const;

  /// CSC strip digis from a particular layer or chamber
  const CSCComparatorDigiContainer& comparatorDigisInDetId(unsigned int) const;
  const CSCComparatorDigiContainer& comparatorDigisInChamber(unsigned int) const;

  /// CSC strip digis from a particular layer or chamber
  const CSCStripDigiContainer& stripDigisInDetId(unsigned int) const;
  const CSCStripDigiContainer& stripDigisInChamber(unsigned int) const;

  /// CSC wire digis from a particular layer or chamber
  const CSCWireDigiContainer& wireDigisInDetId(unsigned int) const;
  const CSCWireDigiContainer& wireDigisInChamber(unsigned int) const;

  // #layers with hits
  int nLayersWithComparatorInChamber(unsigned int) const;
  int nLayersWithStripInChamber(unsigned int) const;
  int nLayersWithWireInChamber(unsigned int) const;

  /// How many CSC chambers with minimum number of layer with digis did this simtrack get?
  int nCoincidenceComparatorChambers(int min_n_layers = 4) const;
  int nCoincidenceStripChambers(int min_n_layers = 4) const;
  int nCoincidenceWireChambers(int min_n_layers = 4) const;

  std::set<int> comparatorsInDetId(unsigned int) const;
  std::set<int> stripsInDetId(unsigned int) const;
  std::set<int> wiregroupsInDetId(unsigned int) const;

  // A non-zero max_gap_to_fill parameter would insert extra half-strips or wiregroups
  // so that gaps of that size or smaller would be filled.
  // E.g., if max_gap_to_fill = 1, and there are digis with strips 4 and 6 in a chamber,
  // the resulting set of digis would be 4,5,6
  std::set<int> comparatorsInChamber(unsigned int, int max_gap_to_fill = 0) const;
  std::set<int> stripsInChamber(unsigned int, int max_gap_to_fill = 0) const;
  std::set<int> wiregroupsInChamber(unsigned int, int max_gap_to_fill = 0) const;

private:

  const SimHitMatcher& sh,

  void matchComparatorsToSimTrack(const CSCComparatorDigiCollection& comparators);
  void matchStripsToSimTrack(const CSCStripDigiCollection& strips);
  void matchWiresToSimTrack(const CSCWireDigiCollection& wires);

  template <class T>
  std::set<unsigned int> selectDetIds(const T &digis, int csc_type) const;

  int minBXComp_, maxBXComp_;
  int minBXComp_, maxBXComp_;
  int minBXWire_, maxBXWire_;

  int matchDeltaComparator_;
  int matchDeltaStrip_;
  int matchDeltaWG_;

  int verboseComparator_;
  int verboseStrip_;
  int verboseWG_;

  std::map<unsigned int, CSCComparatorDigiContainer> detid_to_comparators_;
  std::map<unsigned int, CSCComparatorDigiContainer> chamber_to_comparators_;

  std::map<unsigned int, CSCStripDigiContainer> detid_to_strips_;
  std::map<unsigned int, CSCStripDigiContainer> chamber_to_strips_;

  std::map<unsigned int, CSCWireDigiContainer> detid_to_wires_;
  std::map<unsigned int, CSCWireDigiContainer> chamber_to_wires_;

  CSCComparatorDigiContainer no_comparators_;
  CSCStripDigiContainer no_strips_;
  CSCWireDigiContainer no_wires_;
};

template <class T>
std::set<unsigned int> CSCDigiMatcher::selectDetIds(const T &digis, int csc_type) const
{
  std::set<unsigned int> result;
  for (const auto& p: digis)
    {
      const auto& id = p.first;
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
