#ifndef L1Trigger_CSCTriggerPrimitives_CSCGEMMotherboard_h
#define L1Trigger_CSCTriggerPrimitives_CSCGEMMotherboard_h

#include "L1Trigger/CSCTriggerPrimitives/src/CSCUpgradeMotherboard.h"
#include "L1Trigger/CSCTriggerPrimitives/src/GEMCoPadProcessor.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiCollection.h"
#include "DataFormats/GEMDigi/interface/GEMCoPadDigiCollection.h"

typedef match<GEMPadDigi>   GEMPadDigiId;
typedef matches<GEMPadDigi> GEMPadDigiIds;
typedef matchesBX<GEMPadDigi> GEMPadDigiIdsBX;

typedef match<GEMCoPadDigi>   GEMCoPadDigiId;
typedef matches<GEMCoPadDigi> GEMCoPadDigiIds;
typedef matchesBX<GEMCoPadDigi> GEMCoPadDigiIdsBX;

enum lctTypes{Invalid, ALCTCLCT, ALCTCLCTGEM, ALCTCLCT2GEM, ALCT2GEM, CLCT2GEM};

class CSCGEMMotherboard : public CSCUpgradeMotherboard
{
public:

  CSCGEMMotherboard(unsigned endcap, unsigned station, unsigned sector,
                    unsigned subsector, unsigned chamber,
                    const edm::ParameterSet& conf);

   //Default constructor for testing
  CSCGEMMotherboard();

  virtual ~CSCGEMMotherboard();

  void clear();

  // TMB run modes
  virtual void run(const CSCWireDigiCollection* wiredc,
		   const CSCComparatorDigiCollection* compdc,
		   const GEMPadDigiCollection* gemPads)=0;
  
  void run(const CSCWireDigiCollection* wiredc,
	   const CSCComparatorDigiCollection* compdc,
	   const GEMPadDigiClusterCollection* gemPads);

  /** additional processor for GEMs */
  std::unique_ptr<GEMCoPadProcessor> coPadProcessor;

  /// set CSC and GEM geometries for the matching needs
  void setGEMGeometry(const GEMGeometry *g) { gem_g = g; }

protected:

  virtual const CSCGEMMotherboardLUT* getLUT() const=0;

  int getBX(const GEMPadDigi& p) const;
  int getBX(const GEMCoPadDigi& p) const;

  int getRoll(const GEMPadDigiId& p) const;
  int getRoll(const GEMCoPadDigiId& p) const;
  int getRoll(const CSCALCTDigi&) const;

  float getPad(const GEMPadDigi&) const;
  float getPad(const GEMCoPadDigi&) const;
  float getPad(const CSCCLCTDigi&, enum CSCPart part) const;

  // match ALCT/CLCT to Pad/CoPad
  template <class T> 
  void matchingPads(const CSCALCTDigi& alct, enum CSCPart part, matches<T>&); 
  template <class T> 
  void matchingPads(const CSCCLCTDigi& alct, enum CSCPart part, matches<T>&); 

  template <class S, class T>
  void matchingPads(const S& d1, const S& d2, enum CSCPart part, matches<T>&);

  template <class T>
  void matchingPads(const CSCCLCTDigi& clct1, const CSCALCTDigi& alct1, enum CSCPart part, matches<T>&);

  template <class T>
  void matchingPads(const CSCCLCTDigi& clct1, const CSCCLCTDigi& clct2,
		    const CSCALCTDigi& alct1, const CSCALCTDigi& alct2,
		    enum CSCPart part, matches<T>&);
  
  template <class T>
  T bestMatchingPad(const CSCALCTDigi&, const matches<T>&, enum CSCPart) const;

  template <class T>
  T bestMatchingPad(const CSCCLCTDigi&, const matches<T>&, enum CSCPart) const;

  template <class T>
  T bestMatchingPad(const CSCALCTDigi&, const CSCCLCTDigi&, const matches<T>&, enum CSCPart) const;


  template <class T>
  void correlateLCTsGEM(T& best, T& second, const GEMCoPadDigiIds& coPads, 
			CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2, enum CSCPart);
  template <class T>
  void correlateLCTsGEM(const T& best, const T& second, const GEMCoPadDigi&, const GEMCoPadDigi&,
			CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2, enum CSCPart);

  // specific functions
  CSCCorrelatedLCTDigi constructLCTsGEM(const CSCALCTDigi& alct, const GEMCoPadDigi& gem, enum CSCPart, int i) const;
  CSCCorrelatedLCTDigi constructLCTsGEM(const CSCCLCTDigi& clct, const GEMCoPadDigi& gem, enum CSCPart, int i) const;
  CSCCorrelatedLCTDigi constructLCTsGEM(const CSCALCTDigi& alct, const CSCCLCTDigi& clct,
					const GEMCoPadDigi& gem, enum CSCPart p, int i) const;
  CSCCorrelatedLCTDigi constructLCTsGEM(const CSCALCTDigi& alct, const CSCCLCTDigi& clct,
					const GEMPadDigi& gem, enum CSCPart p, int i) const;
  /*
   * General function to construct integrated stubs from CSC and GEM information.
   * Options are:
   * 1. ALCT-CLCT-GEMPad
   * 2. ALCT-CLCT-GEMCoPad
   * 3. ALCT-GEMCoPad
   * 4. CLCT-GEMCoPad
   */
  CSCCorrelatedLCTDigi constructLCTsGEM(const CSCALCTDigi& alct, const CSCCLCTDigi& clct,
					const GEMPadDigi& gem1,  const GEMCoPadDigi& gem2,
					enum CSCPart p, int i) const;
  
  void retrieveGEMPads(const GEMPadDigiCollection* pads, unsigned id);
  void retrieveGEMCoPads();

  unsigned int findQualityGEM(const CSCALCTDigi&, const CSCCLCTDigi&, int gemlayer) const;

  void printGEMTriggerPads(int bx_start, int bx_stop, enum CSCPart);
  void printGEMTriggerCoPads(int bx_start, int bx_stop, enum CSCPart);

  bool isPadInOverlap(int roll);
  
  void setupGeometry();

  /** Chamber id (trigger-type labels). */
  unsigned gemId; 

  const GEMGeometry* gem_g;
  bool gemGeometryAvailable;

  std::vector<GEMCoPadDigi> gemCoPadV;

  // map< bx , vector<gemid, pad> >
  GEMPadDigiIdsBX pads_;
  GEMCoPadDigiIdsBX coPads_;

  //  deltas used to match to GEM pads
  int maxDeltaBXPad_;
  int maxDeltaBXCoPad_;
  int maxDeltaPadL1_;
  int maxDeltaPadL2_;

  // send LCT old dataformat
  bool useOldLCTDataFormat_;

  // promote ALCT-GEM pattern
  bool promoteALCTGEMpattern_;

  bool promoteALCTGEMquality_;
  bool promoteCLCTGEMquality_;

  // LCT ghostbusting
  bool doLCTGhostBustingWithGEMs_;
};


template <class S>
S CSCGEMMotherboard::bestMatchingPad(const CSCALCTDigi& alct1, const matches<S>& pads, enum CSCPart) const
{
  S result;
  if (not alct1.isValid()) return result;

  // return the first one with the same roll number
  for (const auto& p: pads){
    // protection against corrupted DetIds
    if (DetId(p.first).subdetId() != MuonSubdetId::GEM or DetId(p.first).det() != DetId::Muon) {
      continue;
    }
    if (getRoll(p) == getRoll(alct1)){
      return p.second;
    }
  }
  return result;
}

template <class S>
S CSCGEMMotherboard::bestMatchingPad(const CSCCLCTDigi& clct, const matches<S>& pads, enum CSCPart part) const
{
  S result;
  if (not clct.isValid()) return result;

  // return the pad with the smallest bending angle
  float averagePadNumberCSC = getPad(clct, part);
  float minDeltaPad = 999;
  for (const auto& p: pads){
    // protection against corrupted DetIds
    if (DetId(p.first).subdetId() != MuonSubdetId::GEM or DetId(p.first).det() != DetId::Muon) {
      continue;
    }
    float averagePadNumberGEM = getPad(p.second);
    if (std::abs(averagePadNumberCSC - averagePadNumberGEM) < minDeltaPad){
      minDeltaPad = std::abs(averagePadNumberCSC - averagePadNumberGEM);
      result = p.second;
    }
  }
  return result;
}

template <class S>
S CSCGEMMotherboard::bestMatchingPad(const CSCALCTDigi& alct1, const CSCCLCTDigi& clct1, 
				     const matches<S>& pads, enum CSCPart part) const
{
  S result;
  if (not alct1.isValid() or not clct1.isValid()) return result;

  // return the pad with the smallest bending angle
  float averagePadNumberCSC = getPad(clct1, part);
  float minDeltaPad = 999;
  for (const auto& p: pads){
    // protection against corrupted DetIds
    if (DetId(p.first).subdetId() != MuonSubdetId::GEM or DetId(p.first).det() != DetId::Muon) {
      continue;
    }
    float averagePadNumberGEM = getPad(p.second);
    // add another safety to make sure that the deltaPad is not larger than max value!!!
    if (std::abs(averagePadNumberCSC - averagePadNumberGEM) < minDeltaPad and getRoll(p) == getRoll(alct1)){
      minDeltaPad = std::abs(averagePadNumberCSC - averagePadNumberGEM);
      result = p.second;
    }
  }
  return result;
}

template <class T>
void CSCGEMMotherboard::correlateLCTsGEM(T& bestLCT, T& secondLCT, const GEMCoPadDigiIds& coPads, 
					 CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2, enum CSCPart p)
{
  bool bestValid     = bestLCT.isValid();
  bool secondValid   = secondLCT.isValid();

  // determine best/second
  if (bestValid and !secondValid) secondLCT = bestLCT;
  if (!bestValid and secondValid) bestLCT   = secondLCT;

  // get best matching copad1
  GEMCoPadDigi bestCoPad = bestMatchingPad<GEMCoPadDigi>(bestLCT, coPads, p);
  GEMCoPadDigi secondCoPad = bestMatchingPad<GEMCoPadDigi>(secondLCT, coPads, p);

  correlateLCTsGEM(bestLCT, secondLCT, bestCoPad, secondCoPad, lct1, lct2, p);
}


template <class S, class T>
void CSCGEMMotherboard::matchingPads(const S& d1, const S& d2, 
				     enum CSCPart part, matches<T>& result)
{
  matches<T> p1, p2;
  matchingPads<T>(d1, part, p1);
  matchingPads<T>(d2, part, p2);
  result.reserve(p1.size() + p2.size());
  result.insert(std::end(result), std::begin(p1), std::end(p1));
  result.insert(std::end(result), std::begin(p2), std::end(p2));
}

template <class T>
void CSCGEMMotherboard::matchingPads(const CSCCLCTDigi& clct1, const CSCALCTDigi& alct1, 
				     enum CSCPart part, matches<T>& result)
{
  matches<T> padsClct, padsAlct;
  matchingPads<T>(clct1, part, padsClct);
  matchingPads<T>(alct1, part, padsAlct);
  intersection(padsClct, padsAlct, result);
}

template <class T>
void CSCGEMMotherboard::matchingPads(const CSCCLCTDigi& clct1, const CSCCLCTDigi& clct2,
				     const CSCALCTDigi& alct1, const CSCALCTDigi& alct2,
				     enum CSCPart part, matches<T>& result)
{
  matches<T> padsClct, padsAlct;
  matchingPads<CSCCLCTDigi, T>(clct1, clct2, part, padsClct);
  matchingPads<CSCALCTDigi, T>(alct1, alct2, part, padsAlct);
  intersection(padsClct, padsAlct, result);
}




#endif
