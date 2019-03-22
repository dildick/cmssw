#ifndef GEMCode_GEMValidation_ME0DigiMatcher_h
#define GEMCode_GEMValidation_ME0DigiMatcher_h

/**\class DigiMatcher

 Description: Matching of Digis for SimTrack in ME0

 Original Author:  "Sven Dildick"
*/

#include "GEMCode/GEMValidation/interface/DigiMatcher.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/GEMDigi/interface/ME0DigiPreRecoCollection.h"

#include <vector>
#include <map>
#include <set>

typedef std::vector<ME0DigiPreReco> ME0DigiPreRecoContainer;

class SimHitMatcher;

class ME0DigiMatcher : public DigiMatcher
{
public:

  ME0DigiMatcher(const SimHitMatcher& sh, const edm::EDGetTokenT<ME0DigiPreRecoCollection>& me0DigiInput_);

  ~ME0DigiMatcher();

  // partition ME0 detIds with digis
  std::set<unsigned int> detIds() const;

  // chamber detIds with digis
  std::set<unsigned int> chamberIds() const;

  // superchamber detIds with digis
  std::set<unsigned int> superChamberIds() const;

  // ME0 digis from a particular partition, chamber or superchamber
  const ME0DigiPreRecoContainer& digisInDetId(unsigned int) const;
  const ME0DigiPreRecoContainer& digisInChamber(unsigned int) const;
  const ME0DigiPreRecoContainer& digisInSuperChamber(unsigned int) const;

  // #layers with digis from this simtrack
  int nLayersWithDigisInSuperChamber(unsigned int) const;

  std::set<int> stripNumbersInDetId(unsigned int) const;

  // what unique partitions numbers with digis from this simtrack?
  std::set<int> partitionNumbers() const;

private:

  void matchPreRecoDigisToSimTrack(const ME0DigiPreRecoCollection& digis);

  bool verboseDigi_;
  bool runME0Digi_;
  int minBXME0_, maxBXME0_;
  int matchDeltaStrip_;

  std::map<unsigned int, ME0DigiPreRecoContainer> detid_to_digis_;
  std::map<unsigned int, ME0DigiPreRecoContainer> chamber_to_digis_;
  std::map<unsigned int, ME0DigiPreRecoContainer> superchamber_to_digis_;

  ME0DigiPreRecoContainer no_me0_digis_;
};

#endif
