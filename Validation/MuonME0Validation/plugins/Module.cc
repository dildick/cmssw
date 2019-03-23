#include "FWCore/Framework/interface/MakerMacros.h"

#include "Validation/MuonME0Validation/plugins/ME0HitsValidation.h"
DEFINE_FWK_MODULE (ME0HitsValidation) ;

#include "Validation/MuonME0Validation/plugins/ME0DigisValidation.h"
DEFINE_FWK_MODULE (ME0DigisValidation) ;

#include "Validation/MuonME0Validation/plugins/ME0RecHitsValidation.h"
DEFINE_FWK_MODULE (ME0RecHitsValidation) ;

#include "Validation/MuonME0Validation/plugins/ME0SegmentsValidation.h"
DEFINE_FWK_MODULE (ME0SegmentsValidation) ;
