#ifndef Validation_MuonGEMHits_INTERFACE_GEMValidationUtils_h_
#define Validation_MuonGEMHits_INTERFACE_GEMValidationUtils_h_

#include "DQMServices/Core/interface/DQMStore.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "TString.h"
#include "TSystem.h"

#include <map>
#include <tuple>

class MonitorElement;
class TH1F;
class TH2F;
class TProfile;

typedef std::tuple<Int_t, Int_t> ME2IdsKey;
typedef std::tuple<Int_t, Int_t, Int_t> ME3IdsKey;
typedef std::tuple<Int_t, Int_t, Int_t, Int_t> ME4IdsKey;


namespace GEMUtils {

TString getSuffixName(Int_t region_id);
TString getSuffixName(Int_t region_id, Int_t station_id);
TString getSuffixName(Int_t region_id, Int_t station_id, Int_t layer_id);
TString getSuffixName(Int_t region_id, Int_t station_id,
                      Int_t layer_id, Int_t roll_id);

TString getSuffixName(const ME2IdsKey & key);
TString getSuffixName(const ME3IdsKey & key);
TString getSuffixName(const ME4IdsKey & key);

TString getSuffixTitle(Int_t region_id);
TString getSuffixTitle(Int_t region_id, Int_t station_id);
TString getSuffixTitle(Int_t region_id, Int_t station_id, Int_t layer_id);
TString getSuffixTitle(Int_t region_id, Int_t station_id,
                       Int_t layer_id, Int_t roll_id);

TString getSuffixTitle(const ME2IdsKey & key);
TString getSuffixTitle(const ME3IdsKey & key);
TString getSuffixTitle(const ME4IdsKey & key);

}

#endif // Validation_MuonGEMHits_GEMValidationUtils_h_
