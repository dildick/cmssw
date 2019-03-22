#ifndef Validation_L1T_L1Validator_h
#define Validation_L1T_L1Validator_h

// system include files
#include <memory>
#include <iostream>
#include <string>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Common/interface/Provenance.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/Tau.h"
#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "DQMServices/Core/interface/MonitorElement.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"

#include "Validation/L1T/interface/L1ValidatorHists.h"
#include "Validation/L1T/interface/L1MuMatcher.h"

//
// class declaration
//

class L1Validator : public DQMEDAnalyzer
{
  public:
    explicit L1Validator(const edm::ParameterSet&);
    ~L1Validator() override;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void analyze(const edm::Event&, const edm::EventSetup&) override;

  protected:
    void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;

  private:
    // ----------member data ---------------------------
    std::string _dirName;
    std::string _fileName;

    edm::EDGetTokenT<edm::SimTrackContainer> _SimTrackSource;
    edm::EDGetTokenT<edm::SimVertexContainer> _SimVertexSource;
    edm::EDGetTokenT<reco::GenParticleCollection> _GenSource;
    edm::EDGetTokenT<l1t::MuonBxCollection> _L1MuonBXSource;
    edm::EDGetTokenT<l1t::EGammaBxCollection> _L1EGammaBXSource;
    edm::EDGetTokenT<l1t::TauBxCollection> _L1TauBXSource;
    edm::EDGetTokenT<l1t::JetBxCollection> _L1JetBXSource;
    edm::EDGetTokenT<GenEventInfoProduct>         _srcToken;
    edm::EDGetTokenT<reco::GenJetCollection> _L1GenJetSource;

    L1ValidatorHists _Hists;

    //---------------helper functions------------------
  private:
    const reco::LeafCandidate *FindBest(const reco::GenParticle *, const std::vector<l1extra::L1EmParticle> *, const std::vector<l1extra::L1EmParticle> *);
    const reco::LeafCandidate *FindBest(const reco::GenParticle *, const std::vector<l1extra::L1JetParticle> *, const std::vector<l1extra::L1JetParticle> *);
    const reco::LeafCandidate *FindBest(const reco::GenParticle *, const std::vector<l1extra::L1MuonParticle> *);

    std::unique_ptr<L1MuMatcher> matcher_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

#endif
