#include "FWCore/Framework/interface/MakerMacros.h"

#include "Validation/MuonME0Validation/plugins/ME0HitsValidation.h"
DEFINE_FWK_MODULE(ME0HitsValidation);

#include "Validation/MuonME0Validation/plugins/ME0DigisValidation.h"
DEFINE_FWK_MODULE(ME0DigisValidation);

#include "Validation/MuonME0Validation/plugins/ME0RecHitsValidation.h"
DEFINE_FWK_MODULE(ME0RecHitsValidation);

#include "Validation/MuonME0Validation/plugins/ME0SegmentsValidation.h"
DEFINE_FWK_MODULE(ME0SegmentsValidation);

#include "Validation/MuonME0Validation/plugins/MuonME0DigisHarvestor.h"
DEFINE_FWK_MODULE(MuonME0DigisHarvestor);

#include "Validation/MuonME0Validation/plugins/MuonME0SegHarvestor.h"
DEFINE_FWK_MODULE(MuonME0SegHarvestor);
