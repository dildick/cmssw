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
#include "DataFormats/CSCDigi/interface/CSCCFEBStatusDigiCollection.h"

class CSCViewDigi : public edm::EDAnalyzer {
public:
  explicit CSCViewDigi(const edm::ParameterSet&);
  ~CSCViewDigi() override {}

  void analyze(const edm::Event&, const edm::EventSetup&) override;

  // Fill parameters descriptions
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  bool WiresDump_, AlctDump_, ClctDump_, CorrClctDump_;
  bool StripDump_, ComparatorDump_, RpcDump_, DCCStatusDump_;
  bool DDUStatusDump_, DCCFormatStatusDump_, CFEBStatusDump_;
  bool ClctPreTriggerDump_;

  edm::EDGetTokenT<CSCWireDigiCollection> wd_token;
  edm::EDGetTokenT<CSCStripDigiCollection> sd_token;
  edm::EDGetTokenT<CSCComparatorDigiCollection> cd_token;
  edm::EDGetTokenT<CSCRPCDigiCollection> rd_token;
  edm::EDGetTokenT<CSCALCTDigiCollection> al_token;
  edm::EDGetTokenT<CSCCLCTDigiCollection> cl_token;
  edm::EDGetTokenT<CSCCLCTPreTriggerDigiCollection> clpre_token;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> co_token;
  edm::EDGetTokenT<CSCCFEBStatusDigiCollection> cf_token;
  edm::EDGetTokenT<CSCDCCFormatStatusDigiCollection> st_token;
  edm::EDGetTokenT<CSCDDUStatusDigiCollection> dd_token;
  edm::EDGetTokenT<CSCDCCStatusDigiCollection> dc_token;
};

CSCViewDigi::CSCViewDigi(const edm::ParameterSet& conf) {
  WiresDump_ = conf.getParameter<bool>("WiresDump");
  StripDump_ = conf.getParameter<bool>("StripDump");
  ComparatorDump_ = conf.getParameter<bool>("ComparatorDump");
  RpcDump_ = conf.getParameter<bool>("RpcDump");
  AlctDump_ = conf.getParameter<bool>("AlctDump");
  ClctDump_ = conf.getParameter<bool>("ClctDump");
  ClctPreTriggerDump_ = conf.getParameter<bool>("ClctPreTriggerDump");
  CorrClctDump_ = conf.getParameter<bool>("CorrClctDump");
  CFEBStatusDump_ = conf.getParameter<bool>("CFEBStatusDump");
  DCCFormatStatusDump_ = conf.getParameter<bool>("DCCFormatStatusDump");
  DDUStatusDump_ = conf.getParameter<bool>("DDUStatusDump");
  DCCStatusDump_ = conf.getParameter<bool>("DCCStatusDump");

  // don't consume products you won't print
  // this allows us to run on DIGI-L1 Monte Carlo to test the packing with CFEBs
  if (WiresDump_)
    wd_token = consumes<CSCWireDigiCollection>(conf.getParameter<edm::InputTag>("wireTag"));
  if (StripDump_)
    sd_token = consumes<CSCStripDigiCollection>(conf.getParameter<edm::InputTag>("stripTag"));
  if (ComparatorDump_)
    cd_token = consumes<CSCComparatorDigiCollection>(conf.getParameter<edm::InputTag>("comparatorTag"));
  if (RpcDump_)
    rd_token = consumes<CSCRPCDigiCollection>(conf.getParameter<edm::InputTag>("rpcTag"));
  if (AlctDump_)
    al_token = consumes<CSCALCTDigiCollection>(conf.getParameter<edm::InputTag>("alctTag"));
  if (ClctDump_)
    cl_token = consumes<CSCCLCTDigiCollection>(conf.getParameter<edm::InputTag>("clctTag"));
  if (ClctPreTriggerDump_)
    clpre_token = consumes<CSCCLCTPreTriggerDigiCollection>(conf.getParameter<edm::InputTag>("clctpreTag"));
  if (CorrClctDump_)
    co_token = consumes<CSCCorrelatedLCTDigiCollection>(conf.getParameter<edm::InputTag>("corrclctTag"));
  if (CFEBStatusDump_)
    cf_token = consumes<CSCCFEBStatusDigiCollection>(conf.getParameter<edm::InputTag>("CFEBStatusTag"));
  if (DCCFormatStatusDump_)
    st_token = consumes<CSCDCCFormatStatusDigiCollection>(conf.getParameter<edm::InputTag>("DCCStatusTag"));
  if (DDUStatusDump_)
    dd_token = consumes<CSCDDUStatusDigiCollection>(conf.getParameter<edm::InputTag>("DDUStatusTag"));
  if (DCCStatusDump_)
    dc_token = consumes<CSCDCCStatusDigiCollection>(conf.getParameter<edm::InputTag>("DCCStatusTag"));
}

void CSCViewDigi::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<bool>("WiresDump", true);
  desc.add<bool>("StripDump", true);
  desc.add<bool>("ComparatorDump", true);
  desc.add<bool>("RpcDump", false);
  desc.add<bool>("AlctDump", true);
  desc.add<bool>("ClctDump", true);
  desc.add<bool>("ClctPreTriggerDump", true);
  desc.add<bool>("CorrClctDump", true);
  desc.add<bool>("CFEBStatusDump", false);
  desc.add<bool>("DCCFormatStatusDump", false);
  desc.add<bool>("DDUStatusDump", false);
  desc.add<bool>("DCCStatusDump", false);
  desc.add<edm::InputTag>("wireTag", edm::InputTag("muonCSCDigis", "MuonCSCWireDigi"));
  desc.add<edm::InputTag>("stripTag", edm::InputTag("muonCSCDigis", "MuonCSCStripDigi"));
  desc.add<edm::InputTag>("comparatorTag", edm::InputTag("muonCSCDigis", "MuonCSCComparatorDigi"));
  desc.add<edm::InputTag>("alctTag", edm::InputTag("muonCSCDigis", "MuonCSCALCTDigi"));
  desc.add<edm::InputTag>("clctTag", edm::InputTag("muonCSCDigis", "MuonCSCCLCTDigi"));
  desc.add<edm::InputTag>("clctpreTag", edm::InputTag("simCscTriggerPrimitiveDigis"));
  desc.add<edm::InputTag>("corrclctTag", edm::InputTag("muonCSCDigis", "MuonCSCCorrelatedLCTDigi"));
  desc.add<edm::InputTag>("rpcTag", edm::InputTag("muonCSCDigis", "MuonCSCRPCDigi"));
  desc.add<edm::InputTag>("CFEBStatusTag", edm::InputTag("muonCSCDigis", "MuonCSCCFEBStatusDigi"));
  desc.add<edm::InputTag>("DCCFormatStatusTag", edm::InputTag("muonCSCDigis", "MuonCSCDCCFormatStatusDigi"));
  desc.add<edm::InputTag>("DDUStatusTag", edm::InputTag("muonCSCDigis", "MuonCSCDDUStatusDigi"));
  desc.add<edm::InputTag>("DCCStatusTag", edm::InputTag("muonCSCDigis", "MuonCSCDCCStatusDigi"));
  descriptions.add("cscViewDigiDef", desc);
}

void CSCViewDigi::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  if (WiresDump_) {
    edm::Handle<CSCWireDigiCollection> wires;
    iEvent.getByToken(wd_token, wires);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********WIRES Digis********" << std::endl;
    for (CSCWireDigiCollection::DigiRangeIterator j = wires->begin(); j != wires->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCWireDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCWireDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        const auto& digi = *digiItr;
        std::cout << " CSCWireDigi wg: " << digi.getWireGroup() << ", First Time Bin On: " << digi.getTimeBin() << ", BX " << digi.getWireGroupBX() << std::endl;
      }
    }
  }

  if (StripDump_) {
    edm::Handle<CSCStripDigiCollection> strips;
    iEvent.getByToken(sd_token, strips);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********STRIPS Digis********" << std::endl;
    for (CSCStripDigiCollection::DigiRangeIterator j = strips->begin(); j != strips->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCStripDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCStripDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        const auto& digi = *digiItr;
        std::cout << "CSCStripDigi strip: " << digi.getStrip() << ", CFEB: " << digi.getCFEB() << std::endl;
      }
    }
  }

  if (ComparatorDump_) {
    edm::Handle<CSCComparatorDigiCollection> comparators;
    iEvent.getByToken(cd_token, comparators);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********COMPARATOR Digis********" << std::endl;
    for (CSCComparatorDigiCollection::DigiRangeIterator j = comparators->begin(); j != comparators->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCComparatorDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCComparatorDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (RpcDump_) {
    edm::Handle<CSCRPCDigiCollection> rpcs;
    iEvent.getByToken(rd_token, rpcs);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********RPC Digis********" << std::endl;
    for (CSCRPCDigiCollection::DigiRangeIterator j = rpcs->begin(); j != rpcs->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCRPCDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCRPCDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (AlctDump_) {
    edm::Handle<CSCALCTDigiCollection> alcts;
    iEvent.getByToken(al_token, alcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********ALCT Digis********" << std::endl;
    for (CSCALCTDigiCollection::DigiRangeIterator j = alcts->begin(); j != alcts->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCALCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCALCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (ClctDump_) {
    edm::Handle<CSCCLCTDigiCollection> clcts;
    iEvent.getByToken(cl_token, clcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********CLCT Digis********" << std::endl;
    for (CSCCLCTDigiCollection::DigiRangeIterator j = clcts->begin(); j != clcts->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCCLCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCLCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (ClctPreTriggerDump_) {
    edm::Handle<CSCCLCTPreTriggerDigiCollection> preclcts;
    iEvent.getByToken(clpre_token, preclcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********CLCT PreTrigger Digis********" << std::endl;
    for (CSCCLCTPreTriggerDigiCollection::DigiRangeIterator j = preclcts->begin(); j != preclcts->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCCLCTPreTriggerDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCLCTPreTriggerDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (CorrClctDump_) {
    edm::Handle<CSCCorrelatedLCTDigiCollection> correlatedlcts;
    iEvent.getByToken(co_token, correlatedlcts);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********CorrelatedLCT Digis********" << std::endl;
    for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator j = correlatedlcts->begin(); j != correlatedlcts->end();
         j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCCorrelatedLCTDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCCorrelatedLCTDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (DCCFormatStatusDump_) {
    edm::Handle<CSCDCCFormatStatusDigiCollection> statusdigis;
    iEvent.getByToken(st_token, statusdigis);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********STATUS Digis********" << std::endl;
    for (CSCDCCFormatStatusDigiCollection::DigiRangeIterator j = statusdigis->begin(); j != statusdigis->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCDCCFormatStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDCCFormatStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (DDUStatusDump_) {
    edm::Handle<CSCDDUStatusDigiCollection> DDUstatusdigi;
    iEvent.getByToken(dd_token, DDUstatusdigi);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********DDU STATUS Digis********" << std::endl;
    for (CSCDDUStatusDigiCollection::DigiRangeIterator j = DDUstatusdigi->begin(); j != DDUstatusdigi->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCDDUStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDDUStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }

  if (DCCStatusDump_) {
    edm::Handle<CSCDCCStatusDigiCollection> DCCstatusdigi;
    iEvent.getByToken(dc_token, DCCstatusdigi);
    std::cout << std::endl;
    std::cout << "Event " << iEvent.id() << std::endl;
    std::cout << std::endl;
    std::cout << "********DCC STATUS Digis********" << std::endl;
    for (CSCDCCStatusDigiCollection::DigiRangeIterator j = DCCstatusdigi->begin(); j != DCCstatusdigi->end(); j++) {
      std::cout << " in " << CSCDetId((*j).first) << std::endl;
      std::vector<CSCDCCStatusDigi>::const_iterator digiItr = (*j).second.first;
      std::vector<CSCDCCStatusDigi>::const_iterator last = (*j).second.second;
      for (; digiItr != last; ++digiItr) {
        std::cout << *digiItr << std::endl;
      }
    }
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCViewDigi);
