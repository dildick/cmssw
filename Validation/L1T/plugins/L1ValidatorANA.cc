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
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "Validation/L1T/interface/L1MuMatcher.h"

#include "TFile.h"


class L1ValidatorANA : public edm::EDAnalyzer
{
public:
  explicit L1ValidatorANA(const edm::ParameterSet&);

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  void analyze(const edm::Event&, const edm::EventSetup&) override;

private:

  edm::EDGetTokenT<edm::SimTrackContainer> _SimTrackSource;
  edm::EDGetTokenT<edm::SimVertexContainer> _SimVertexSource;

  std::unique_ptr<L1MuMatcher> matcher_;
};

L1ValidatorANA::L1ValidatorANA(const edm::ParameterSet& iConfig)
{
  _SimTrackSource = consumes<edm::SimTrackContainer>(iConfig.getParameter<edm::InputTag>("SimTrackSource"));
  _SimVertexSource = consumes<edm::SimVertexContainer>(iConfig.getParameter<edm::InputTag>("SimVertexSource"));

  matcher_.reset(new L1MuMatcher(iConfig, consumesCollector()));
}


void L1ValidatorANA::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  std::cout << "Analyze event" << std::endl;

  using namespace edm;
  using namespace std;

  matcher_->init(iEvent, iSetup);

  edm::Handle<edm::SimTrackContainer> sim_tracks;
  iEvent.getByToken(_SimTrackSource, sim_tracks);
  const edm::SimTrackContainer& sim_track = *sim_tracks.product();

  edm::Handle<edm::SimVertexContainer> sim_vertices;
  iEvent.getByToken(_SimVertexSource, sim_vertices);
  const edm::SimVertexContainer & sim_vert = *sim_vertices.product();

  int trk_no=0;
  std::cout << "Processing SimTracks " << sim_track.size() << std::endl;

  for (const auto& t: sim_track) {
    std::cout << "Processing SimTrack " << trk_no+1 << std::endl;

    /*
    if (t.noVertex()) continue;
    if (t.noGenpart()) continue;

    // only muons
    if (std::abs(t.type()) != 13) continue;

    // pt selection
    if (t.momentum().pt() < 2) continue;

    const float eta(std::abs(t.momentum().eta()));
    if (eta > 2.4) continue;
    */
    std::cout << "pt(GeV/c) = " << t.momentum().pt() << ", eta = " << t.momentum().eta()
              << ", phi = " << t.momentum().phi() << ", Q = " << t.charge() << " PDGId = " << t.type() << std::endl;

    matcher_->match(t, sim_vert[t.vertIndex()]);


  }
  return;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void L1ValidatorANA::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1ValidatorANA);
