#ifndef Validation_MuonHits_Helpers_h
#define Validation_MuonHits_Helpers_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "DataFormats/MuonDetId/interface/DTWireId.h"

/// Muon Subsystem
enum MuonType {DT= 1, CSC=2, RPC=3, GEM=4, ME0=5};

/// CSC chamber types, according to CSCDetId::iChamberType()
enum CSCType {CSC_ALL = 0, CSC_ME11,
	      CSC_ME1a, CSC_ME1b, CSC_ME12, CSC_ME13, CSC_ME21,
	      CSC_ME22, CSC_ME31, CSC_ME32, CSC_ME41, CSC_ME42, GEM_ME0,
	      CSC_ME1, CSC_ME2, CSC_ME3, CSC_ME4};

/// GEM chamber types
enum GEMType {GEM_ALL = 0,
	      GEM_ME11, GEM_ME21};

/// RPC endcap chamber types
enum RPCType {RPC_ALL = 0,
	      RPC_ME12, RPC_ME13, RPC_ME22, RPC_ME23, RPC_ME31,
	      RPC_ME32, RPC_ME33, RPC_ME41, RPC_ME42, RPC_ME43,
	      RPC_MB01,  RPC_MB02,  RPC_MB03,  RPC_MB04,
	      RPC_MB11p, RPC_MB12p, RPC_MB13p, RPC_MB14p,
	      RPC_MB21p, RPC_MB22p, RPC_MB23p, RPC_MB24p,
	      RPC_MB11n, RPC_MB12n, RPC_MB13n, RPC_MB14n,
	      RPC_MB21n, RPC_MB22n, RPC_MB23n, RPC_MB24n,
	      RPC_ME1, RPC_ME2, RPC_ME3, RPC_ME4,
	      RPC_MB1, RPC_MB2, RPC_MB3, RPC_MB4};

/// DT chamber types
enum DTType {DT_ALL = 0,
	     DT_MB01,  DT_MB02,  DT_MB03,  DT_MB04,
	     DT_MB11p, DT_MB12p, DT_MB13p, DT_MB14p,
	     DT_MB21p, DT_MB22p, DT_MB23p, DT_MB24p,
	     DT_MB11n, DT_MB12n, DT_MB13n, DT_MB14n,
	     DT_MB21n, DT_MB22n, DT_MB23n, DT_MB24n,
	     DT_MB1, DT_MB2, DT_MB3, DT_MB4};

namespace
{
inline bool
is_dt(unsigned int detId)
{
  return (DetId(detId)).subdetId() == MuonSubdetId::DT;
}

inline bool
is_gem(unsigned int detId)
{
  return (DetId(detId)).subdetId() == MuonSubdetId::GEM;
}

inline bool
is_csc(unsigned int detId)
{
  return (DetId(detId)).subdetId() == MuonSubdetId::CSC;
}

inline bool
is_rpc(unsigned int detId)
{
  return (DetId(detId)).subdetId() == MuonSubdetId::RPC;
}

inline bool
is_me0(unsigned int detId)
{
  return (DetId(detId)).subdetId() == MuonSubdetId::ME0;
}

int
chamber(const DetId& id);

unsigned int
gemDetFromCSCDet(unsigned int id,int layer);

float
cscHalfStripWidth(CSCDetId id);

std::pair<unsigned int, unsigned int>
gemDetsFromCSCDet(unsigned int id);

// return MuonType for a particular DetId
int toGEMType(int st, int ri);
int toRPCType(int re, int st, int ri);
int toDTType(int wh, int st);
int toCSCType(int st, int ri);

std::string toGEMTypeString(int st, int ri);
std::string toRPCTypeString(int re, int st, int ri);
std::string toDTTypeString(int wh, int st);
std::string toCSCTypeString(int st, int ri);

}

#endif
