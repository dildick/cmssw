#ifndef L1Trigger_CSCTriggerPrimitives_CSCComparatorCodePosLUT
#define L1Trigger_CSCTriggerPrimitives_CSCComparatorCodePosLUT

#include <fstream>
#include <sstream>
#include <bitset>

#include "CondFormats/L1TObjects/interface/LUT.h"

class CSCComparatorCodePosLUT : public LUT {
 public:
  CSCComparatorCodePosLUT(){};
  explicit CSCComparatorCodePosLUT(const std::string&);
  explicit CSCComparatorCodePosLUT(l1t::LUT*);
  ~CSCComparatorCodePosLUT() override{};

  int lookup(int pat, int code) const;
  int lookupPacked(int in) const override;

  int hashInput(int pt, int code) const;
  void unHashInput(int input, int& pat, int& code) const;

  int checkedInput(unsigned in, unsigned maxWidth) const;

 private:
  int m_patInWidth;
  int m_codeInWidth;
};

#endif
