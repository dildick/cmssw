#ifndef _CSCFAKEDBCROSSTALK_H
#define _CSCFAKEDBCROSSTALK_H

#include <memory>
#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/CSCObjects/interface/CSCDBCrosstalk.h"
#include "CondFormats/DataRecord/interface/CSCDBCrosstalkRcd.h"
#include <DataFormats/MuonDetId/interface/CSCDetId.h>

class CSCFakeDBCrosstalk: public edm::ESProducer, public edm::EventSetupRecordIntervalFinder  {
 public:
  CSCFakeDBCrosstalk(const edm::ParameterSet&);
  ~CSCFakeDBCrosstalk();
  
  float mean,min,minchi;
  int seed;long int M;
  int ii,jj,iii,jjj;

  void prefillDBFakeCrosstalk();
  
  typedef const  CSCDBCrosstalk * ReturnType;
  
  ReturnType produceDBCrosstalk(const CSCDBCrosstalkRcd&);
  
 private:
  // ----------member data ---------------------------
  void setIntervalFor(const edm::eventsetup::EventSetupRecordKey &, const edm::IOVSyncValue&, edm::ValidityInterval & );
  CSCDBCrosstalk *cndbcrosstalk ;
  
};

#endif

