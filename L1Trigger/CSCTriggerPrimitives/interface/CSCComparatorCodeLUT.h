#ifndef L1Trigger_CSCTriggerPrimitives_CSCComparatorCodeLUT_h
#define L1Trigger_CSCTriggerPrimitives_CSCComparatorCodeLUT_h

#include <map>
#include <pair>

class CSCComparatorCodeLUT {
public:

  typedef static const std::map<int, std::map<int, std::pair<float, float> > > cclut;

  // static member lut
   cclut compcode_lut;
};

#endif
