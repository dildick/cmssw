#ifndef CSCTriggerPrimitives_CSCRPCMotherboard_h
#define CSCTriggerPrimitives_CSCRPCMotherboard_h

/** \class CSCMotherboardME3141R0C
 *
 * Extended CSCMotherboard for ME3141 TMB upgrade
 *
 * \author Sven Dildick March 2014
 *
 * Based on CSCMotherboard code
 *
 */

#include "L1Trigger/CSCTriggerPrimitives/src/CSCUpgradeMotherboard.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"

typedef match<RPCDigi>   RPCDigiId;
typedef matches<RPCDigi> RPCDigiIds;
typedef matchesBX<RPCDigi> RPCDigiIdsBX;

class CSCGeometry;
class CSCChamber;
class RPCGeometry;

class CSCRPCMotherboard : public CSCUpgradeMotherboard
{
 public:

  /** Normal constructor. */
  CSCRPCMotherboard(unsigned endcap, unsigned station, unsigned sector, 
		    unsigned subsector, unsigned chamber,
		    const edm::ParameterSet& conf);

  /** Default destructor. */
  ~CSCRPCMotherboard() override;

  /** Run function for normal usage.  Runs cathode and anode LCT processors,
      takes results and correlates into CorrelatedLCT. */
  void run(const CSCWireDigiCollection* wiredc, 
           const CSCComparatorDigiCollection* compdc, 
           const RPCDigiCollection* rpcDigis);
  
  /// set CSC and RPC geometries for the matching needs
  void setRPCGeometry(const RPCGeometry *g) { rpc_g = g; }
  void setupGeometry();

  /* store the CLCTs found earlier */
  std::vector<CSCCLCTDigi> clctV;

  /* readout the two best LCTs in this CSC */
  std::vector<CSCCorrelatedLCTDigi> readoutLCTs() const;

 private: 

  /* access to the LUTs needed for matching */
  const CSCRPCMotherboardLUT* getLUT() const {return tmbLUT_;}
  const CSCRPCMotherboardLUT* tmbLUT_;

  /** Clears correlated LCT and passes clear signal on to cathode and anode
      LCT processors. */
  void clear();

  void retrieveRPCDigis(const RPCDigiCollection* digis, unsigned id);
  void printRPCTriggerDigis(int minBX, int maxBx);

  // match CLCT to RPC digi
  void matchingRPCDigis(const CSCCLCTDigi&, RPCDigiIds&) const;  

  // match ALCT to RPC digi
  void matchingRPCDigis(const CSCALCTDigi&, RPCDigiIds&) const;  

  // find common matches between an ALCT and CLCT
  void matchingRPCDigis(const CSCCLCTDigi&, const CSCALCTDigi&, RPCDigiIds&) const;

  // find all matching digis to a pair of ALCT and a pair of CLCT
  void matchingRPCDigis(const CSCCLCTDigi&, const CSCCLCTDigi&,
			const CSCALCTDigi&, const CSCALCTDigi&, RPCDigiIds&) const;


  // find the matching digis to a pair of ALCT/CLCT
  template <class S>
  void matchingRPCDigis(const S& d1, const S& d2, RPCDigiIds&) const;
  
  // find the best matching digi to an ALCT
  RPCDigiId bestMatchingDigi(const CSCALCTDigi&, const RPCDigiIds&) const;

  // find the best matching digi to an CLCT
  RPCDigiId bestMatchingDigi(const CSCCLCTDigi&, const RPCDigiIds&) const;

  // find the best matching digi to an ALCT and CLCT
  RPCDigiId bestMatchingDigi(const CSCALCTDigi&, const CSCCLCTDigi&, 
			     const RPCDigiIds&) const;

  // correlate ALCTs/CLCTs with a set of matching RPC digis
  // use this function when the best matching digis are not clear yet
  // the template is ALCT or CLCT
  template <class T>  
  void correlateLCTsRPC(T& best, T& second, const RPCDigiIds& coDigis, 
			CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2) const;

  void correlateLCTsRPC(CSCALCTDigi& bestALCT, CSCALCTDigi& secondALCT,
			CSCCLCTDigi& bestCLCT, CSCCLCTDigi& secondCLCT,
			const RPCDigiIds& digis,
			CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2) const;

  /* correlate a pair of ALCTs and a pair of CLCTs with matched digis 
     the output is up to two LCTs in a sector of ME31/ME41 */
  // the template is ALCT or CLCT
  template <class T>
  void correlateLCTsRPC(const T& best, const T& second, 
			const RPCDigiId&, const RPCDigiId&,
			CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2) const;

  // construct LCT from CLCT and RPC digi
  // third argument is the LCT number (1 or 2)
  CSCCorrelatedLCTDigi constructLCTsRPC(const CSCCLCTDigi& clct, 
					const RPCDigiId& rpc, int) const;

  // construct LCT from ALCT and RPC digi
  // third argument is the LCT number (1 or 2)
  CSCCorrelatedLCTDigi constructLCTsRPC(const CSCALCTDigi& alct, 
					const RPCDigiId& rpc, int) const;

  // construct LCT from ALCT,CLCT and RPC digi
  // fourth argument is the LCT number (1 or 2)
  CSCCorrelatedLCTDigi constructLCTsRPC(const CSCALCTDigi& alct, 
					const CSCCLCTDigi& clct, 
					const RPCDigiId& digi, int) const; 

  // quality of the LCT in case RPC digi is present
  unsigned int findQualityRPC(const CSCALCTDigi& aLCT, 
			      const CSCCLCTDigi& cLCT, 
			      bool hasRPC) const;

  // aux functions to get BX and position of a digi
  int getBX(const RPCDigi& p) const;
  int getRoll(const RPCDigiId& p) const;
  int getRoll(const CSCALCTDigi& alct) const;

  float getStrip(const RPCDigiId& p) const;
  float getStrip(const CSCCLCTDigi& clct) const;

  bool isValid(const RPCDigiId& p) const;
  
  /** Chamber id (trigger-type labels). */
  unsigned rpcId; 

  /** for the case when more than 2 LCTs/BX are allowed;
      maximum match window = 15 */
  LCTContainer allLCTs;

  const RPCGeometry* rpc_g;
  bool rpcGeometryAvailable;

  //  deltas used to match to RPC digis
  int maxDeltaBX_;
  int maxDeltaStrip_;

  bool useOldLCTDataFormat_;

  // drop low quality stubs if they don't have RPCs
  bool dropLowQualityCLCTsNoRPCs_;

  bool buildLCTfromALCTandRPC_;
  bool buildLCTfromCLCTandRPC_;
  bool buildLCTfromLowQstubandRPC_;

  bool promoteALCTRPCquality_;
  bool promoteALCTRPCpattern_;
  bool promoteCLCTRPCquality_;

  // map< bx , vector<rpcid, pad> >
  RPCDigiIdsBX rpcDigis_;
};

template <class S>
void CSCRPCMotherboard::matchingRPCDigis(const S& d1, const S& d2, RPCDigiIds& result) const
{
  RPCDigiIds p1,p2;
  matchingRPCDigis(d1, p1);
  matchingRPCDigis(d2, p2);
  result.reserve(p1.size() + p2.size());
  result.insert(std::end(result), std::begin(p1), std::end(p1));
  result.insert(std::end(result), std::begin(p2), std::end(p2));
}

template <class S>  
void CSCRPCMotherboard::correlateLCTsRPC(S& bestLCT,
					 S& secondLCT,
					 const RPCDigiIds& digis, 
					 CSCCorrelatedLCTDigi& lct1,
					 CSCCorrelatedLCTDigi& lct2) const
{
  const bool bestValid     = bestLCT.isValid();
  const bool secondValid   = secondLCT.isValid();

  // check which one is valid, which one is not
  if (bestValid and !secondValid) secondLCT = bestLCT;
  if (!bestValid and secondValid) bestLCT   = secondLCT;

  // get best matching copad1
  const RPCDigiId& bestDigi = bestMatchingDigi(bestLCT, digis);
  const RPCDigiId& secondDigi = bestMatchingDigi(secondLCT, digis);

  correlateLCTsRPC(bestLCT, secondLCT, bestDigi, secondDigi, lct1, lct2);
}


#endif
