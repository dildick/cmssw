#ifndef L1Trigger_CSCTriggerPrimitives_CSCComparatorCodePosLUT
#define L1Trigger_CSCTriggerPrimitives_CSCComparatorCodePosLUT

#include <fstream>
#include <sstream>
#include <bitset>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CondFormats/L1TObjects/interface/LUT.h"

class CSCComparatorCodePosLUT : public l1t::LUT
{
 public:
  CSCComparatorCodePosLUT(){};
  explicit CSCComparatorCodePosLUT(const std::string&);
  explicit CSCComparatorCodePosLUT(l1t::LUT*);
  ~CSCComparatorCodePosLUT() {}

  int lookup(int pat, int code) const;
  int lookupPacked(int in) const;

  // populates the map.
  void initialize();

  int hashInput(int pt, int code) const;
  void unHashInput(int input, int& pat, int& code) const;

  int checkedInput(unsigned in, unsigned maxWidth) const;

  // I/O functions
  void save(std::ofstream& output);
  int load(const std::string& inFileName);

 private:
  unsigned m_totalInWidth;
  unsigned m_outWidth;
  int m_patInWidth;
  int m_codeInWidth;
  bool m_initialized;
};

#endif
