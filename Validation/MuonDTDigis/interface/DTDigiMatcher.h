#ifndef GEMCode_GEMValidation_DTDigiMatcher_h
#define GEMCode_GEMValidation_DTDigiMatcher_h

/**\class DigiMatcher

 Description: Matching of Digis for SimTrack in DT

 Original Author:  Sven Dildick
*/

#include "GEMCode/GEMValidation/interface/DigiMatcher.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/DTDigi/interface/DTDigiCollection.h"

#include <vector>
#include <map>
#include <set>

class SimHitMatcher;

class DTDigiMatcher : public DigiMatcher
{
public:

  typedef std::vector<DTDigi> DTDigiContainer;

  DTDigiMatcher(const SimHitMatcher& sh, const edm::EDGetTokenT<DTDigiCollection>& dtDigiInput_);
  
  ~DTDigiMatcher();

  // partition GEM detIds with digis
  std::set<unsigned int> detIds(int dt_type = DT_ALL) const;
  // chamber detIds with digis
  std::set<unsigned int> layerIds(int dt_type = DT_ALL) const;
  // superchamber detIds with digis
  std::set<unsigned int> superLayerIds(int dt_type = DT_ALL) const;
  // chamber detIds with digis
  std::set<unsigned int> chamberIds(int dt_type = DT_ALL) const;

  //DT digis from a particular partition, chamber or superchamber
  const DTDigiContainer& digisInDetId(unsigned int) const;
  const DTDigiContainer& digisInLayer(unsigned int) const;
  const DTDigiContainer& digisInSuperLayer(unsigned int) const;
  const DTDigiContainer& digisInChamber(unsigned int) const;

  // #tubes with digis in layer from this simtrack
  int nTubesWithDigisInLayer(unsigned int) const;
  // #layers with digis from this simtrack
  int nLayersWithDigisInSuperLayer(unsigned int) const;
  // #layers with digis from this simtrack
  int nSuperLayersWithDigisInChamber(unsigned int) const;

  // wire numbers from this simtrack in a detId
  std::set<int> wireNumbersInDetId(unsigned int) const;

private:

  void matchDigisToSimTrack(const DTDigiCollection&);

  std::set<unsigned int> selectDetIds(const std::map<unsigned int, DTDigiContainer>&, int) const;


  bool verboseDigi_;
  bool runDTDigi_;
  int minBXDT_, maxBXDT_;
  int matchDeltaWire_;

  std::map<unsigned int, DTDigiContainer> detid_to_digis_;
  std::map<unsigned int, DTDigiContainer> layer_to_digis_;
  std::map<unsigned int, DTDigiContainer> superLayer_to_digis_;
  std::map<unsigned int, DTDigiContainer> chamber_to_digis_;

  DTDigiContainer no_dt_digis_;
};

#endif
