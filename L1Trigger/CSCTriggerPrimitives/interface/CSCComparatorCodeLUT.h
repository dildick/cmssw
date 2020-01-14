#ifndef L1Trigger_CSCTriggerPrimitives_CSCComparatorCodeLUT_h
#define L1Trigger_CSCTriggerPrimitives_CSCComparatorCodeLUT_h

#include <map>
#include <utility>

class CSCComparatorCodeLUT {
public:

  typedef std::map<int, std::map<int, std::pair<float, float> > > cclut;

  // static member lut
   static const cclut compcode_lut;
};

#endif
