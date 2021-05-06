// Original Author:  Alexandre Sakharov
//         Created:  Sun May 10 15:43:28 CEST 2009

#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "EventFilter/CSCRawToDigi/interface/CSCALCTHeader.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCRPCDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTPreTriggerDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCDCCFormatStatusDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCDDUStatusDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCDCCStatusDigiCollection.h"

class CSCViewDigi : public edm::EDAnalyzer {
public:
  explicit CSCViewDigi(const edm::ParameterSet&);
  ~CSCViewDigi() override {}

  void analyze(const edm::Event&, const edm::EventSetup&) override;

  // Fill parameters descriptions
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  bool WiresDigiDump_, AlctDigiDump_, ClctDigiDump_, CorrClctDigiDump_;
  bool StripDigiDump_, ComparatorDigiDump_, RpcDigiDump_, StatusDigiDump_;
  bool DDUStatusDigiDump_, DCCStatusDigiDump_;
  bool ClctPreTriggerDigiDump_;

  edm::EDGetTokenT<CSCWireDigiCollection> wd_token;
  edm::EDGetTokenT<CSCStripDigiCollection> sd_token;
  edm::EDGetTokenT<CSCComparatorDigiCollection> cd_token;
  edm::EDGetTokenT<CSCRPCDigiCollection> rd_token;
  edm::EDGetTokenT<CSCALCTDigiCollection> al_token;
  edm::EDGetTokenT<CSCCLCTDigiCollection> cl_token;
  edm::EDGetTokenT<CSCCLCTPreTriggerDigiCollection> clpre_token;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;
  edm::EDGetTokenT<CSCDCCFormatStatusDigiCollection> st_token;
  edm::EDGetTokenT<CSCDDUStatusDigiCollection> dd_token;
  edm::EDGetTokenT<CSCDCCStatusDigiCollection> dc_token;
};

CSCViewDigi::CSCViewDigi(const edm::ParameterSet& conf) {
  WiresDigiDump_ = conf.getParameter<bool>("WiresDigiDump");
  StripDigiDump_ = conf.getParameter<bool>("StripDigiDump");
  ComparatorDigiDump_ = conf.getParameter<bool>("ComparatorDigiDump");
  RpcDigiDump_ = conf.getParameter<bool>("RpcDigiDump");
  AlctDigiDump_ = conf.getParameter<bool>("AlctDigiDump");
  ClctDigiDump_ = conf.getParameter<bool>("ClctDigiDump");
  ClctPreTriggerDigiDump_ = conf.getParameter<bool>("ClctPreDigiDump");
  CorrClctDigiDump_ = conf.getParameter<bool>("CorrClctDigiDump");
  StatusDigiDump_ = conf.getParameter<bool>("StatusDigiDump");
  DDUStatusDigiDump_ = conf.getParameter<bool>("DDUStatus");
  DCCStatusDigiDump_ = conf.getParameter<bool>("DCCStatus");

  // don't consume products you won't print
  // this allows us to run on DIGI-L1 Monte Carlo to test the packing with CFEBs
  if (WiresDigiDump_)
    wd_token = consumes<CSCWireDigiCollection>(conf.getParameter<edm::InputTag>("wireDigiTag"));
  if (StripDigiDump_)
    sd_token = consumes<CSCStripDigiCollection>(conf.getParameter<edm::InputTag>("stripDigiTag"));
  if (ComparatorDigiDump_)
    cd_token = consumes<CSCComparatorDigiCollection>(conf.getParameter<edm::InputTag>("comparatorDigiTag"));
  if (RpcDigiDump_)
    rd_token = consumes<CSCRPCDigiCollection>(conf.getParameter<edm::InputTag>("rpcDigiTag"));
  if (AlctDigiDump_)
    al_token = consumes<CSCALCTDigiCollection>(conf.getParameter<edm::InputTag>("alctDigiTag"));
  if (ClctDigiDump_)
    cl_token = consumes<CSCCLCTDigiCollection>(conf.getParameter<edm::InputTag>("clctDigiTag"));
  if (ClctPreTriggerDigiDump_)
    clpre_token = consumes<CSCCLCTPreTriggerDigiCollection>(conf.getParameter<edm::InputTag>("clctpreDigiTag"));
  if (CorrClctDigiDump_)
    co_token = consumes<CSCCorrelatedLCTDigiCollection>(conf.getParameter<edm::InputTag>("corrclctDigiTag"));
  if (StatusDigiDump_)
    st_token = consumes<CSCDCCFormatStatusDigiCollection>(conf.getParameter<edm::InputTag>("statusDigiTag"));
  if (DDUStatusDigiDump_)
    dd_token = consumes<CSCDDUStatusDigiCollection>(conf.getParameter<edm::InputTag>("DDUstatusDigiTag"));
  if (DCCStatusDigiDump_)
    dc_token = consumes<CSCDCCStatusDigiCollection>(conf.getParameter<edm::InputTag>("DCCstatusDigiTag"));
}

void CSCViewDigi::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<bool>("WiresDigiDump", true);
  desc.add<bool>("StripDigiDump", true);
  desc.add<bool>("ComparatorDigiDump", true);
  desc.add<bool>("RpcDigiDump", true);
  desc.add<bool>("AlctDigiDump", true);
  desc.add<bool>("ClctDigiDump", true);
  desc.add<bool>("ClctPreDigiDump", true);
  desc.add<bool>("CorrClctDigiDump", true);
  desc.add<bool>("StatusDigiDump", true);
  desc.add<bool>("DDUStatus", true);
  desc.add<bool>("DCCStatus", true);
  desc.add<edm::InputTag>("wireDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCWireDigi"));
  desc.add<edm::InputTag>("stripDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCStripDigi"));
  desc.add<edm::InputTag>("comparatorDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCComparatorDigi"));
  desc.add<edm::InputTag>("alctDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCALCTDigi"));
  desc.add<edm::InputTag>("clctDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCCLCTDigi"));
  desc.add<edm::InputTag>("clctPreDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCCorrelatedLCTDigi"));
  desc.add<edm::InputTag>("corrclctDigiTag", edm::InputTag("simCscTriggerPrimitiveDigis"));
  desc.add<edm::InputTag>("rpcDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCRPCDigi"));
  desc.add<edm::InputTag>("statusDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCDCCFormatStatusDigi"));
  desc.add<edm::InputTag>("statusCFEBTag", edm::InputTag("muonCSCDigis", "MuonCSCCFEBStatusDigi"));
  desc.add<edm::InputTag>("DDUstatusDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCDDUStatusDigi"));
  desc.add<edm::InputTag>("DCCstatusDigiTag", edm::InputTag("muonCSCDigis", "MuonCSCDCCStatusDigi"));
  descriptions.add("cscViewDigiDef", desc);
}

void CSCViewDigi::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  if (WiresDigiDump_) {
    edm::Handle<CSCWireDigiCollection> wires;
    iEvent.getByToken(wd_token, wires);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********WIRES Digis********" << std::endl;
    for (CSCWireDigiCollection::DigiRangeIterator j = wires->begin(); j != wires->end(); j++) {
      std::cout << "Wire digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCWireDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCWireDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (StripDigiDump_) {
    edm::Handle<CSCStripDigiCollection> strips;
    iEvent.getByToken(sd_token, strips);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********STRIPS Digis********" << std::endl;
    for (CSCStripDigiCollection::DigiRangeIterator j = strips->begin(); j != strips->end(); j++) {
      std::cout << "Strip digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCStripDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCStripDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (ComparatorDigiDump_) {
    edm::Handle<CSCComparatorDigiCollection> comparators;
    iEvent.getByToken(cd_token, comparators);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********COMPARATOR Digis********" << std::endl;
    for (CSCComparatorDigiCollection::DigiRangeIterator j = comparators->begin(); j != comparators->end(); j++) {
      std::cout << "Comparator digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCComparatorDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCComparatorDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (RpcDigiDump_) {
    edm::Handle<CSCRPCDigiCollection> rpcs;
    iEvent.getByToken(rd_token, rpcs);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********RPC Digis********" << std::endl;
    for (CSCRPCDigiCollection::DigiRangeIterator j = rpcs->begin(); j != rpcs->end(); j++) {
      std::cout << "RPC digis from " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCRPCDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCRPCDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (AlctDigiDump_) {
    edm::Handle<CSCALCTDigiCollection> alcts;
    iEvent.getByToken(al_token, alcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********ALCT Digis********" << std::endl;
    for (CSCALCTDigiCollection::DigiRangeIterator j = alcts->begin(); j != alcts->end(); j++) {
      std::vector<CSCALCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCALCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (ClctDigiDump_) {
    edm::Handle<CSCCLCTDigiCollection> clcts;
    iEvent.getByToken(cl_token, clcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********CLCT Digis********" << std::endl;
    for (CSCCLCTDigiCollection::DigiRangeIterator j = clcts->begin(); j != clcts->end(); j++) {
      std::vector<CSCCLCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCLCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (ClctPreTriggerDigiDump_) {
    edm::Handle<CSCCLCTPreTriggerDigiCollection> preclcts;
    iEvent.getByToken(clpre_token, preclcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********CLCT PreTrigger Digis********" << std::endl;
    for (CSCCLCTPreTriggerDigiCollection::DigiRangeIterator j = preclcts->begin(); j != preclcts->end(); j++) {
      std::vector<CSCCLCTPreTriggerDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCLCTPreTriggerDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (CorrClctDigiDump_) {
    edm::Handle<CSCCorrelatedLCTDigiCollection> correlatedlcts;
    iEvent.getByToken(co_token, correlatedlcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********CorrelatedLCT Digis********" << std::endl;
    for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator j = correlatedlcts->begin(); j != correlatedlcts->end();
         j++) {
      std::vector<CSCCorrelatedLCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCorrelatedLCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (StatusDigiDump_) {
    edm::Handle<CSCDCCFormatStatusDigiCollection> statusdigis;
    iEvent.getByToken(st_token, statusdigis);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********STATUS Digis********" << std::endl;
    for (CSCDCCFormatStatusDigiCollection::DigiRangeIterator j = statusdigis->begin(); j != statusdigis->end(); j++) {
      std::vector<CSCDCCFormatStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDCCFormatStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (DDUStatusDigiDump_) {
    edm::Handle<CSCDDUStatusDigiCollection> DDUstatusdigi;
    iEvent.getByToken(dd_token, DDUstatusdigi);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********DDU STATUS Digis********" << std::endl;
    for (CSCDDUStatusDigiCollection::DigiRangeIterator j = DDUstatusdigi->begin(); j != DDUstatusdigi->end(); j++) {
      std::vector<CSCDDUStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDDUStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }

  if (DCCStatusDigiDump_) {
    edm::Handle<CSCDCCStatusDigiCollection> DCCstatusdigi;
    iEvent.getByToken(dc_token, DCCstatusdigi);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********DCC STATUS Digis********" << std::endl;
    for (CSCDCCStatusDigiCollection::DigiRangeIterator j = DCCstatusdigi->begin(); j != DCCstatusdigi->end(); j++) {
      std::vector<CSCDCCStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDCCStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        digiItr->print();
      }
    }
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCViewDigi);
