#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTPreTriggerDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "DataFormats/CSCDigi/interface/CSCConstants.h"

class CSCPackerUnpackerUnitTest : public edm::EDAnalyzer {
public:
  explicit CSCPackerUnpackerUnitTest(const edm::ParameterSet&);
  ~CSCPackerUnpackerUnitTest() override {}

  void analyze(const edm::Event&, const edm::EventSetup&) override;

  // Fill parameters descriptions
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:

  static const unsigned max_tests = 6;
  static const unsigned max_chambers = 540;

  void analyzeChamber(const CSCDetId& cscId,
                      unsigned& iChamber,
                      bool testRan[max_tests][max_chambers],
                      bool testOK[max_tests][max_chambers],
                      const CSCWireDigiCollection& wd,
                      const CSCWireDigiCollection& wdu,
                      const CSCStripDigiCollection& sd,
                      const CSCStripDigiCollection& sdu,
                      const CSCComparatorDigiCollection& cd,
                      const CSCComparatorDigiCollection& cdu,
                      const CSCALCTDigiCollection& alct,
                      const CSCCLCTDigiCollection& clct,
                      const CSCCLCTPreTriggerDigiCollection& pre,
                      const CSCCorrelatedLCTDigiCollection& lct) const;

  /// takes layer ID, converts to chamber ID, switching ME1A to ME11
  CSCDetId chamberID(const CSCDetId& cscDetId) const;

  template <typename LCTCollection>
  bool accept(const CSCDetId& cscId, const LCTCollection& lcts, int bxMin, int bxMax, int nominalBX) const;

  // need to specialize for pretriggers
  bool accept(const CSCDetId& cscId,
              const CSCCLCTPreTriggerDigiCollection& lcts,
              int bxMin,
              int bxMax,
              int nominalBX,
              bool preTriggerInCFEB[CSCConstants::MAX_CFEBS_RUN2]) const;

  template <typename DIGICollection>
  unsigned nDigis(const CSCDetId& cscId, const DIGICollection& digis) const;

  void nDigisCFEB(const CSCDetId& cscId, const CSCStripDigiCollection& digis, unsigned digisInCFEB[CSCConstants::MAX_CFEBS_RUN2]) const;
  unsigned getNCFEBs(unsigned type) const;

  unsigned nWireDigis(const CSCDetId& cscId, const CSCWireDigiCollection& wiredc) const;
  unsigned nStripDigis(const CSCDetId& cscId, const CSCStripDigiCollection& stripdc) const;
  unsigned nComparatorDigis(const CSCDetId& cscId, const CSCComparatorDigiCollection& compdc) const;

  unsigned nWireDigisLayer(const CSCDetId& cscId, const CSCWireDigiCollection& wiredc) const;
  unsigned nStripDigisLayer(const CSCDetId& cscId, const CSCStripDigiCollection& stripdc) const;
  unsigned nComparatorDigisLayer(const CSCDetId& cscId, const CSCComparatorDigiCollection& compdc) const;

  edm::EDGetTokenT<CSCWireDigiCollection> wd_token;
  edm::EDGetTokenT<CSCWireDigiCollection> wd_unpacked_token;
  edm::EDGetTokenT<CSCStripDigiCollection> sd_token;
  edm::EDGetTokenT<CSCStripDigiCollection> sd_unpacked_token;
  edm::EDGetTokenT<CSCComparatorDigiCollection> cd_token;
  edm::EDGetTokenT<CSCComparatorDigiCollection> cd_unpacked_token;
  edm::EDGetTokenT<CSCALCTDigiCollection> al_token;
  edm::EDGetTokenT<CSCCLCTDigiCollection> cl_token;
  edm::EDGetTokenT<CSCCLCTPreTriggerDigiCollection> clpre_token;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;

  int alctWindowMin_;
  int alctWindowMax_;
  int clctWindowMin_;
  int clctWindowMax_;
  int preTriggerWindowMin_;
  int preTriggerWindowMax_;
};

CSCPackerUnpackerUnitTest::CSCPackerUnpackerUnitTest(const edm::ParameterSet& conf) :
  alctWindowMin_(conf.getParameter<int>("alctWindowMin")),
  alctWindowMax_(conf.getParameter<int>("alctWindowMax")),
  clctWindowMin_(conf.getParameter<int>("clctWindowMin")),
  clctWindowMax_(conf.getParameter<int>("clctWindowMax")),
  preTriggerWindowMin_(conf.getParameter<int>("preTriggerWindowMin")),
  preTriggerWindowMax_(conf.getParameter<int>("preTriggerWindowMax"))
{
  wd_token = consumes<CSCWireDigiCollection>(conf.getParameter<edm::InputTag>("wireTag"));
  sd_token = consumes<CSCStripDigiCollection>(conf.getParameter<edm::InputTag>("stripTag"));
  cd_token = consumes<CSCComparatorDigiCollection>(conf.getParameter<edm::InputTag>("comparatorTag"));
  wd_unpacked_token = consumes<CSCWireDigiCollection>(conf.getParameter<edm::InputTag>("wireUnpackedTag"));
  sd_unpacked_token = consumes<CSCStripDigiCollection>(conf.getParameter<edm::InputTag>("stripUnpackedTag"));
  cd_unpacked_token = consumes<CSCComparatorDigiCollection>(conf.getParameter<edm::InputTag>("comparatorUnpackedTag"));
  al_token = consumes<CSCALCTDigiCollection>(conf.getParameter<edm::InputTag>("alctTag"));
  cl_token = consumes<CSCCLCTDigiCollection>(conf.getParameter<edm::InputTag>("clctTag"));
  clpre_token = consumes<CSCCLCTPreTriggerDigiCollection>(conf.getParameter<edm::InputTag>("clctpreTag"));
  co_token = consumes<CSCCorrelatedLCTDigiCollection>(conf.getParameter<edm::InputTag>("corrclctTag"));
}

void CSCPackerUnpackerUnitTest::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  // digi collections
  desc.add<edm::InputTag>("wireTag", edm::InputTag("simMuonCSCDigis", "MuonCSCWireDigi"));
  desc.add<edm::InputTag>("stripTag", edm::InputTag("simMuonCSCDigis", "MuonCSCStripDigi"));
  desc.add<edm::InputTag>("comparatorTag", edm::InputTag("simMuonCSCDigis", "MuonCSCComparatorDigi"));
  desc.add<edm::InputTag>("wireUnpackedTag", edm::InputTag("muonCSCDigis", "MuonCSCWireDigi"));
  desc.add<edm::InputTag>("stripUnpackedTag", edm::InputTag("muonCSCDigis", "MuonCSCStripDigi"));
  desc.add<edm::InputTag>("comparatorUnpackedTag", edm::InputTag("muonCSCDigis", "MuonCSCComparatorDigi"));
  // trigger collections
  desc.add<edm::InputTag>("alctTag", edm::InputTag("simCscTriggerPrimitiveDigis"));
  desc.add<edm::InputTag>("clctTag", edm::InputTag("simCscTriggerPrimitiveDigis"));
  desc.add<edm::InputTag>("clctpreTag", edm::InputTag("simCscTriggerPrimitiveDigis"));
  desc.add<edm::InputTag>("corrclctTag", edm::InputTag("simCscTriggerPrimitiveDigis"));
  desc.add<int32_t>("alctWindowMin", -3);
  desc.add<int32_t>("alctWindowMax", 3);
  desc.add<int32_t>("clctWindowMin", -3);
  desc.add<int32_t>("clctWindowMax", 3);
  desc.add<int32_t>("preTriggerWindowMin", -3);
  desc.add<int32_t>("preTriggerWindowMax", 1);
  descriptions.add("cscPackerUnpackerUnitTestDef", desc);
}

void CSCPackerUnpackerUnitTest::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // handles, tokens
  edm::Handle<CSCWireDigiCollection> wires;
  iEvent.getByToken(wd_token, wires);

  edm::Handle<CSCStripDigiCollection> strips;
  iEvent.getByToken(sd_token, strips);

  edm::Handle<CSCComparatorDigiCollection> comparators;
  iEvent.getByToken(cd_token, comparators);

  edm::Handle<CSCWireDigiCollection> wires_unpacked;
  iEvent.getByToken(wd_unpacked_token, wires_unpacked);

  edm::Handle<CSCStripDigiCollection> strips_unpacked;
  iEvent.getByToken(sd_unpacked_token, strips_unpacked);

  edm::Handle<CSCComparatorDigiCollection> comparators_unpacked;
  iEvent.getByToken(cd_unpacked_token, comparators_unpacked);

  edm::Handle<CSCALCTDigiCollection> alcts;
  iEvent.getByToken(al_token, alcts);

  edm::Handle<CSCCLCTDigiCollection> clcts;
  iEvent.getByToken(cl_token, clcts);

  edm::Handle<CSCCLCTPreTriggerDigiCollection> preclcts;
  iEvent.getByToken(clpre_token, preclcts);

  edm::Handle<CSCCorrelatedLCTDigiCollection> lcts;
  iEvent.getByToken(co_token, lcts);

  const int min_endcap = CSCDetId::minEndcapId();
  const int max_endcap = CSCDetId::maxEndcapId();
  const int min_station = CSCDetId::minStationId();
  const int max_station = CSCDetId::maxStationId();
  const int min_sector = CSCTriggerNumbering::minTriggerSectorId();
  const int max_sector = CSCTriggerNumbering::maxTriggerSectorId();
  const int min_subsector = CSCTriggerNumbering::minTriggerSubSectorId();
  const int max_subsector = CSCTriggerNumbering::maxTriggerSubSectorId();
  const int min_chamber = CSCTriggerNumbering::minTriggerCscId();
  const int max_chamber = CSCTriggerNumbering::maxTriggerCscId();

  bool testOK[max_tests][max_chambers] = {{false}};
  bool testRan[max_tests][max_chambers] = {{false}};

  unsigned iChamber = 0;

  // Initializing boards.
  for (int endc = min_endcap; endc <= max_endcap; endc++) {
    for (int stat = min_station; stat <= max_station; stat++) {
      int numsubs = ((stat == 1) ? max_subsector : 1);
      for (int sect = min_sector; sect <= max_sector; sect++) {
        for (int subs = min_subsector; subs <= numsubs; subs++) {
          for (int cham = min_chamber; cham <= max_chamber; cham++) {
            int ring = CSCTriggerNumbering::ringFromTriggerLabels(stat, cham);

            // actual chamber number =/= trigger chamber number
            int chid = CSCTriggerNumbering::chamberFromTriggerLabels(sect, subs, stat, cham);

            // 0th layer means whole chamber.
            CSCDetId cscDetId(endc, stat, ring, chid, 0);

            analyzeChamber(cscDetId,
                           iChamber,
                           testRan,
                           testOK,
                           *wires,
                           *wires_unpacked,
                           *strips,
                           *strips_unpacked,
                           *comparators,
                           *comparators_unpacked,
                           *alcts,
                           *clcts,
                           *preclcts,
                           *lcts);
          }
        }
      }
    }
  }

  std::string explanations[max_tests] = {
    "Test 1: check that an ALCT in this chamber kept the wire digis",
    "Test 2: check that no digis were kept if there was no ALCT ",
    "Test 3: count the number of wire digis before and after unpacking",
    "Test 4: check that an CCLT in this chamber kept the comp digis",
    "Test 5: check that no digis were kept if there was no CCLT ",
    "Test 6: count the number of comp digis before and after unpacking",
  };

  for (unsigned i = 0; i < max_tests; i++) {
    unsigned nTestsRun = 0;
    unsigned nTestsOK = 0;
    for (unsigned j = 0; j < max_chambers; j++) {
      if (testRan[i][j]) {
        nTestsRun++;
        if (testOK[i][j]) {
          nTestsOK++;
        }
      }
    }
    std::cout << explanations[i] << "\tTests Run: " << nTestsRun << ", Tests Pass: " << nTestsOK << std::endl;
  }
}

void CSCPackerUnpackerUnitTest::analyzeChamber(const CSCDetId& cscDetId,
                                               unsigned& iChamber,
                                               bool testRan[max_tests][max_chambers],
                                               bool testOK[max_tests][max_chambers],
                                               const CSCWireDigiCollection& wires,
                                               const CSCWireDigiCollection& wires_unpacked,
                                               const CSCStripDigiCollection& strips,
                                               const CSCStripDigiCollection& strips_unpacked,
                                               const CSCComparatorDigiCollection& comparators,
                                               const CSCComparatorDigiCollection& comparators_unpacked,
                                               const CSCALCTDigiCollection& alcts,
                                               const CSCCLCTDigiCollection& clcts,
                                               const CSCCLCTPreTriggerDigiCollection& preclcts,
                                               const CSCCorrelatedLCTDigiCollection& lcts) const
{
  CSCDetId chamberId = chamberID(cscDetId);

  // readout condition for wires: L1A + ALCT
  bool hasALCT = accept(cscDetId, alcts, alctWindowMin_, alctWindowMax_, CSCConstants::ALCT_CENTRAL_BX);

  // readout condition for comparators in (O)TMB; L1A + CLCT
  bool hasCLCT = accept(cscDetId, clcts, clctWindowMin_, clctWindowMax_, CSCConstants::CLCT_CENTRAL_BX);

  // determine where the pretriggers are
  bool preTriggerInCFEB[CSCConstants::MAX_CFEBS_RUN2];

  // readout condition for strips: L1A + preCLCT in CFEB
  bool hasPreCLCT = accept(cscDetId,
                           preclcts,
                           preTriggerWindowMin_,
                           preTriggerWindowMax_,
                           CSCConstants::CLCT_CENTRAL_BX,
                           preTriggerInCFEB);

  unsigned numWireDigis = nWireDigis(cscDetId, wires);
  unsigned numWireDigisUnpacked = nWireDigis(cscDetId, wires);

  unsigned numCompDigis = nComparatorDigis(cscDetId, comparators);
  unsigned numCompDigisUnpacked = nComparatorDigis(cscDetId, comparators_unpacked);

  unsigned numStripDigis = nStripDigis(cscDetId, strips);
  unsigned numStripDigisUnpacked = nStripDigis(cscDetId, strips_unpacked);

  unsigned nStripDigisCFEB[CSCConstants::MAX_CFEBS_RUN2];
  nDigisCFEB(chamberId, strips, nStripDigisCFEB);

  unsigned nStripDigisUnpackedCFEB[CSCConstants::MAX_CFEBS_RUN2];
  nDigisCFEB(chamberId, strips_unpacked, nStripDigisUnpackedCFEB);

  /// ALCT tests
  if (numWireDigis) {
    // test 1: check that an ALCT in this chamber kept the wire digis
    testRan[0][iChamber] = true;
    if (!hasALCT or (hasALCT and numWireDigisUnpacked > 0))
      testOK[0][iChamber] = true;

    // test 2: check that no digis were kept if there was no ALCT
    testRan[1][iChamber] = true;
    if (!hasALCT and numWireDigisUnpacked == 0)
      testOK[1][iChamber] = true;

    // test 3: count the number of wire digis before and after unpacking
    testRan[2][iChamber] = true;
    if (numWireDigis >= numWireDigisUnpacked)
      testOK[2][iChamber] = true;
  }

  /// CLCT tests
  if (numCompDigis) {

    // test 4: check that a CLCT in this chamber kept the comp digis
    testRan[3][iChamber] = true;
    if (!hasCLCT or (hasCLCT and numCompDigisUnpacked > 0))
      testOK[3][iChamber] = true;

    // test 5: check that no digis were kept if there was no CLCT
    testRan[4][iChamber] = true;
    if (!hasCLCT and numCompDigisUnpacked == 0)
      testOK[4][iChamber] = true;

    // test 6: count the number of comp digis before and after unpacking
    testRan[5][iChamber] = true;
    if (numCompDigis >= numCompDigisUnpacked)
      testOK[5][iChamber] = true;
  }

  /// CFEB tests
  if (numStripDigis) {

    /*
      const unsigned maxCFEBs = getNCFEBs(cscDetId.iChamberType());

      for (unsigned i = 0; i < maxCFEBs; i++) {

      // test 7: check that a CLCT in this chamber kept the comp digis
      testRan[5+i][iChamber] = true;
      testOK[5+i][iChamber] = preTriggerInCFEB[i] and (nStripDigisUnpackedCFEB[i] > 0);

      // test 8: check that no digis were kept if there was no CLCT
      testRan[5+i+7][iChamber] = true;
      testOK[5+i+7][iChamber] = (nStripDigisCFEB[i] > 0) and !preTriggerInCFEB[i] and (nStripDigisUnpackedCFEB[i] > 0);

      // test 9: count the number of comp digis before and after unpacking
      testRan[5+i+14][iChamber] = true;
      testOK[5+i+14][iChamber] = nStripDigisCFEB[i] == nStripDigisUnpackedCFEB[i] and preTriggerInCFEB[i];
      }
    */
  }

  iChamber++;
}

/// takes layer ID, converts to chamber ID, switching ME1A to ME11
CSCDetId CSCPackerUnpackerUnitTest::chamberID(const CSCDetId& cscDetId) const {
  CSCDetId chamberId = cscDetId.chamberId();
  if (chamberId.ring() == 4) {
    chamberId = CSCDetId(chamberId.endcap(), chamberId.station(), 1, chamberId.chamber(), 0);
  }
  return chamberId;
}

template <typename LCTCollection>
bool CSCPackerUnpackerUnitTest::accept(const CSCDetId& cscId, const LCTCollection& lcts, int bxMin, int bxMax, int nominalBX) const {
  CSCDetId chamberId = chamberID(cscId);
  auto lctRange = lcts.get(chamberId);
  for (auto lctItr = lctRange.first; lctItr != lctRange.second; ++lctItr) {
    int bx = lctItr->getBX() - nominalBX;
    if (bx >= bxMin && bx <= bxMax) {
      return true;
    }
  }
  return false;
}

// need to specialize for pretriggers
bool CSCPackerUnpackerUnitTest::accept(const CSCDetId& cscId,
                                       const CSCCLCTPreTriggerDigiCollection& lcts,
                                       int bxMin,
                                       int bxMax,
                                       int nominalBX,
                                       bool preTriggerInCFEB[CSCConstants::MAX_CFEBS_RUN2]) const {

  bool atLeastOnePreTrigger = false;
  CSCDetId chamberId = chamberID(cscId);
  auto lctRange = lcts.get(chamberId);
  bool result = false;
  // check for pretriggers in a time window and save the location of those pretriggers
  for (auto lctItr = lctRange.first; lctItr != lctRange.second; ++lctItr) {
    int bx = lctItr->getBX() - nominalBX;
    if (bx >= bxMin && bx <= bxMax) {
      atLeastOnePreTrigger = true;
      // save the location of all pretriggers
      preTriggerInCFEB[lctItr->getCFEB()] = true;
    }
  }
  return atLeastOnePreTrigger;
}

template <typename DIGICollection>
unsigned CSCPackerUnpackerUnitTest::nDigis(const CSCDetId& cscId, const DIGICollection& digis) const {
  unsigned nDigis = 0;
  auto digiRange = digis.get(cscId);
  for (auto digiItr = digiRange.first; digiItr != digiRange.second; ++digiItr) {
    nDigis++;
  }
  if (cscId.station()==1 and cscId.ring()==4) {
    auto digiRange = digis.get(CSCDetId(cscId.endcap(), cscId.station(), 4, cscId.chamber()));
    for (auto digiItr = digiRange.first; digiItr != digiRange.second; ++digiItr) {
      nDigis++;
    }
  }
  return nDigis;
};

void CSCPackerUnpackerUnitTest::nDigisCFEB(const CSCDetId& cscId, const CSCStripDigiCollection& digis, unsigned digisInCFEB[CSCConstants::MAX_CFEBS_RUN2]) const {
  unsigned nDigis = 0;
  auto digiRange = digis.get(cscId);
  for (auto digiItr = digiRange.first; digiItr != digiRange.second; ++digiItr) {
    CSCStripDigi digi = *digiItr;
    digisInCFEB[digi.getCFEB()]++;
  }
  if (cscId.station()==1 and cscId.ring()==4) {
    auto digiRange = digis.get(CSCDetId(cscId.endcap(), cscId.station(), 4, cscId.chamber()));
    for (auto digiItr = digiRange.first; digiItr != digiRange.second; ++digiItr) {
      CSCStripDigi digi = *digiItr;
      digisInCFEB[digi.getCFEB() + CSCConstants::NUM_CFEBS_ME1B]++;
    }
  }
};

unsigned CSCPackerUnpackerUnitTest::getNCFEBs(unsigned type) const {
  unsigned cfebs[10] = {3, 7, 5, 4, 5, 5, 5, 5, 5, 5};
  return cfebs[type];
}

unsigned CSCPackerUnpackerUnitTest::nWireDigis(const CSCDetId& detid, const CSCWireDigiCollection& wiredc) const {
  unsigned ndigis = 0;
  for (int i_layer = 0; i_layer < CSCConstants::NUM_LAYERS; i_layer++) {
    CSCDetId ldetid(detid.endcap(), detid.station(), detid.ring(), detid.chamber(), i_layer + 1);
    ndigis += nWireDigisLayer(ldetid, wiredc);;
  }
  return ndigis;
}

unsigned CSCPackerUnpackerUnitTest::nWireDigisLayer(const CSCDetId& detid, const CSCWireDigiCollection& wiredc) const {
  unsigned ndigis = 0;
  const auto rwired = wiredc.get(detid);
  for (auto digiIt = rwired.first; digiIt != rwired.second; ++digiIt) {
    ndigis++;
  }
  return ndigis;
}

unsigned CSCPackerUnpackerUnitTest::nStripDigis(const CSCDetId& detid, const CSCStripDigiCollection& stripdc) const {
  unsigned ndigis = 0;
  for (int i_layer = 0; i_layer < CSCConstants::NUM_LAYERS; i_layer++) {

    CSCDetId ldetid(detid.endcap(), detid.station(), detid.ring(), detid.chamber(), i_layer + 1);
    ndigis += nStripDigisLayer(ldetid, stripdc);

    if (detid.station() == 1 and detid.ring() == 1) {
      CSCDetId ldetid_me1a(detid.endcap(), detid.station(), 4, detid.chamber(), i_layer + 1);
      ndigis += nStripDigisLayer(ldetid_me1a, stripdc);
    }
  }
  return ndigis;
}

unsigned CSCPackerUnpackerUnitTest::nStripDigisLayer(const CSCDetId& detid, const CSCStripDigiCollection& stripdc) const {
  unsigned ndigis = 0;
  const auto rstripd = stripdc.get(detid);
  for (auto digiIt = rstripd.first; digiIt != rstripd.second; ++digiIt) {
    ndigis++;
  }
  return ndigis;
}

unsigned CSCPackerUnpackerUnitTest::nComparatorDigis(const CSCDetId& detid, const CSCComparatorDigiCollection& stripdc) const {
  unsigned ndigis = 0;
  for (int i_layer = 0; i_layer < CSCConstants::NUM_LAYERS; i_layer++) {

    CSCDetId ldetid(detid.endcap(), detid.station(), detid.ring(), detid.chamber(), i_layer + 1);
    ndigis += nComparatorDigisLayer(ldetid, stripdc);

    if (detid.station() == 1 and detid.ring() == 1) {
      CSCDetId ldetid_me1a(detid.endcap(), detid.station(), 4, detid.chamber(), i_layer + 1);
      ndigis += nComparatorDigisLayer(ldetid_me1a, stripdc);
    }
  }
  return ndigis;
}

unsigned CSCPackerUnpackerUnitTest::nComparatorDigisLayer(const CSCDetId& detid, const CSCComparatorDigiCollection& stripdc) const {
  unsigned ndigis = 0;
  const auto rstripd = stripdc.get(detid);
  for (auto digiIt = rstripd.first; digiIt != rstripd.second; ++digiIt) {
    ndigis++;
  }
  return ndigis;
}


//define this as a plug-in
DEFINE_FWK_MODULE(CSCPackerUnpackerUnitTest);
