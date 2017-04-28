#ifndef L1Trigger_CSCTriggerPrimitives_CSCIntegratedMotherboardLUTGenerator_h
#define L1Trigger_CSCTriggerPrimitives_CSCIntegratedMotherboardLUTGenerator_h

#include "L1Trigger/CSCCommonTrigger/interface/CSCTriggerGeometry.h"
#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

#include <vector>
#include <map>

class CSCIntegratedMotherboardLUTGenerator
{
public:

  CSCIntegratedMotherboardLUTGenerator() {}
  ~CSCIntegratedMotherboardLUTGenerator() {}
  
  /// set CSC and GEM geometries for the matching needs
  void setCSCGeometry(const CSCGeometry *g) { csc_g = g; }
  void setGEMGeometry(const GEMGeometry *g) { gem_g = g; }
  void setRPCGeometry(const RPCGeometry *g) { rpc_g = g; }
  
  /// generate and print LUT
  void generateLUTs(unsigned e, unsigned s, unsigned se, unsigned sb, unsigned c) const;
  void generateLUTsME11(unsigned e, unsigned se, unsigned sb, unsigned c) const;
  void generateLUTsME21(unsigned e, unsigned se, unsigned sb, unsigned c) const;
  void generateLUTsME3141(unsigned e, unsigned s, unsigned se, unsigned sb, unsigned c) const;
  int assignRoll(const std::map<int,std::pair<double,double> >&, double eta) const;
  
 private:
  const CSCGeometry* csc_g;
  const GEMGeometry* gem_g;
  const RPCGeometry* rpc_g;
};

#endif
