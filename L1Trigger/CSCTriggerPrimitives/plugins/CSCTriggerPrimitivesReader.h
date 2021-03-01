#ifndef L1Trigger_CSCTriggerPrimitives_CSCTriggerPrimitivesReader_h
#define L1Trigger_CSCTriggerPrimitives_CSCTriggerPrimitivesReader_h

/** \class CSCTriggerPrimitivesReader
 *
 * Basic analyzer class which accesses ALCTs, CLCTs, and correlated LCTs
 * and plot various quantities.
 *
 * \author Slava Valuev, UCLA.
 *
 *
 */

#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "L1Trigger/CSCCommonTrigger/interface/CSCConstants.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCLCTPreTriggerDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"

#include "L1Trigger/CSCTriggerPrimitives/interface/CSCCathodeLCTProcessor.h"  // TMB07

#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <sstream>

class CSCGeometry;
class CSCBadChambers;
class TFile;

struct TreePerStub {
  void init(int run, int event, int lumi);  // initialize to default values
  TTree *bookTree(TTree *t, const std::string &name = "TreePerStub");

  Int_t t_EventNumberAnalyzed;
  Int_t t_Run;
  Int_t t_Lumi;
  Int_t t_Event;
  Int_t t_nStubs;
  Int_t t_nStubs_readout;  //only for emulation
  Int_t t_nStubs_ME119;
  Int_t t_nStubs_ME11no911;
  Int_t t_nStubs_noMEpm11;

  Int_t t_chamber;
  Int_t t_sector;
  Int_t t_ring;
  Int_t t_endcap;
  Int_t t_station;
  Int_t t_chambertype;
  Int_t t_nComp;
  // 7 wide: counts of all hits in a certain time window
  std::vector<Int_t> t_compTimes;
  std::vector<Int_t> t_wireTimes;
  Int_t t_nWire;
};

struct MyStubComparison {
  void init(int run, int event, int lumi);  // initialize to default values
  TTree *bookTree(TTree *t, const std::string &name = "Stub_compare");

  Int_t nEvents;
  Int_t nRun;
  Int_t nLumi;
  Int_t nEvent;
  Bool_t firstfill;
  Int_t totStubs_data;
  Int_t totStubs_emul;
  Int_t totStubs_emul_readout;
  Int_t nStub_data;
  Int_t nStub_emul;
  Int_t chamber;
  Int_t ring;
  Int_t endcap;
  Int_t station;
  Int_t chambertype;
  Bool_t has_data;
  Bool_t has_emul;

  Int_t quality_data;
  Int_t bend_data;
  Int_t bx_data;
  Int_t quality_emul;
  Int_t bend_emul;
  Int_t bx_emul;
  Int_t bx_corr_emul;  //corrected
  Int_t npretrig;
  Int_t quality_pretrig;
  Int_t maxquality_pretrig;
  Int_t bend_pretrig;
  Int_t bx_pretrig;
  Int_t key_hs_pretrig;
  Int_t pattern_pretrig;
  Int_t maxpattern_pretrig;
  Int_t fullbx_data;
  Int_t fullbx_emul;
  Int_t pattern_data;
  Int_t pattern_emul;
  Bool_t WGcrossHS_data;
  Bool_t WGcrossHS_emul;
  Int_t key_WG_data;
  Int_t key_WG_emul;
  Int_t key_hs_data;
  Int_t key_hs_emul;
  Int_t trknmb_data;
  Int_t trknmb_emul;
  Float_t dphi_data;
  Float_t dphi_emul;
  Float_t eta_data;
  Float_t eta_emul;
  Float_t phi_data;
  Float_t phi_emul;
};

class CSCTriggerPrimitivesReader : public edm::EDAnalyzer {
public:
  /// Constructor
  explicit CSCTriggerPrimitivesReader(const edm::ParameterSet &conf);

  /// Destructor
  ~CSCTriggerPrimitivesReader() override;

  /// Does the job
  void analyze(const edm::Event &event, const edm::EventSetup &setup) override;

  /// Write to ROOT file, make plots, etc.
  void endJob() override;

  int chamberSerial(CSCDetId id);
  int chamberIX(CSCDetId id);
  int chamberIXi(CSCDetId id);
  void HotWires(const CSCWireDigiCollection *wireDigis);

private:
  int eventsAnalyzed;        // event number
  bool debug;                // on/off switch
  std::string rootFileName;  // root file name

  // Run number, Event number
  int Run_;
  int Event_;
  int Lumi_;

  // Cache geometry for current event
  const CSCGeometry *geom_;

  // Cache conditions data for bad chambers
  const CSCBadChambers *badChambers_;

  // Define which LCTs are present in the input file.  This will determine the
  // workflow of the Reader.
  bool mcTruthIn_;
  bool dataLctsIn_;
  bool emulLctsIn_;

  bool gangedME1a;

  // Flag to plot or not plot ME1/A as a separate chamber.
  bool plotME1A;

  // Flag to plot or not plot ME4/2.
  bool plotME42;

  // handles
  edm::Handle<CSCComparatorDigiCollection> comp_data_;
  edm::Handle<CSCWireDigiCollection> wire_data_;
  edm::Handle<CSCALCTDigiCollection> alcts_data_;
  edm::Handle<CSCCLCTDigiCollection> clcts_data_;
  edm::Handle<CSCCorrelatedLCTDigiCollection> lcts_tmb_data_;
  edm::Handle<CSCCorrelatedLCTDigiCollection> lcts_mpc_data_;

  edm::Handle<reco::GenParticleCollection> genParticles_;
  edm::Handle<edm::PSimHitContainer> simHits_;
  edm::Handle<CSCComparatorDigiCollection> comp_emul_;
  edm::Handle<CSCWireDigiCollection> wire_emul_;
  edm::Handle<CSCALCTDigiCollection> alcts_emul_;
  edm::Handle<CSCCLCTDigiCollection> clcts_emul_;
  edm::Handle<CSCCLCTPreTriggerDigiCollection> pretrigs_emul_;
  edm::Handle<CSCCorrelatedLCTDigiCollection> lcts_tmb_emul_;
  edm::Handle<CSCCorrelatedLCTDigiCollection> lcts_mpc_emul_;

  // simulation
  edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken_;
  edm::EDGetTokenT<edm::PSimHitContainer> simHit_token_;
  edm::EDGetTokenT<CSCWireDigiCollection> wire_e_token_;
  edm::EDGetTokenT<CSCComparatorDigiCollection> comp_e_token_;
  edm::EDGetTokenT<CSCALCTDigiCollection> alcts_e_token_;
  edm::EDGetTokenT<CSCCLCTDigiCollection> clcts_e_token_;
  edm::EDGetTokenT<CSCCLCTPreTriggerDigiCollection> pretrigs_e_token_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> lcts_tmb_e_token_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> lcts_mpc_e_token_;

  // data
  edm::EDGetTokenT<CSCWireDigiCollection> wire_d_token_;
  edm::EDGetTokenT<CSCComparatorDigiCollection> comp_d_token_;
  edm::EDGetTokenT<CSCALCTDigiCollection> alcts_d_token_;
  edm::EDGetTokenT<CSCCLCTDigiCollection> clcts_d_token_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> lcts_tmb_d_token_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> lcts_mpc_d_token_;

  // a prefix for results ps files
  std::string resultsFileNamesPrefix_;

  // whether to perform check against known "bad chambers" list
  bool checkBadChambers_;

  // not data vs. mc, but mc vs mc
  bool dataIsAnotherMC_;

  // The file which will store the histos
  // TFile *theFile;

  enum trig_cscs { MAX_ENDCAPS = 2, MAX_STATIONS = 4, CSC_TYPES = 10 };
  enum { MAXPAGES = 20 };     // max. number of pages in postscript files
  static const double TWOPI;  // 2.*pi
  static const int MAXSTUBS = 100;
  static const int MAXLLPS = 2;

  // Various useful constants
  static const std::string csc_type[CSC_TYPES];
  static const std::string csc_type_plus[CSC_TYPES];
  static const std::string csc_type_minus[CSC_TYPES];
  static const int NCHAMBERS[CSC_TYPES];
  static const int MAX_WG[CSC_TYPES];
  static const int MAX_HS[CSC_TYPES];
  static const int ptype_TMB07[CSCConstants::NUM_CLCT_PATTERNS];

  static const int lut_wg_vs_hs_me1b[48][2];
  static const int lut_wg_vs_hs_me1a[48][2];
  static const int lut_wg_vs_hs_me1ag[48][2];
  // LCT counters
  static int numALCT;
  static int numCLCT;
  static int numLCTTMB;
  static int numLCTMPC;

  static bool bookedHotWireHistos;
  static bool bookedALCTHistos;
  static bool bookedCLCTHistos;
  static bool bookedLCTTMBHistos;
  static bool bookedLCTMPCHistos;

  static bool bookedCompHistos;

  static bool bookedResolHistos;
  static bool bookedEfficHistos;

  static bool printps;

  //modified alct functions
  void resetALCTreeBranches();
  void enableALCTreeBranches();

  void setRootStyle();

  void bookHotWireHistos();
  void bookALCTHistos();
  void bookCLCTHistos();
  void bookLCTTMBHistos();
  void bookLCTMPCHistos();
  void fillALCTHistos(const CSCALCTDigiCollection *alcts);
  void fillCLCTHistos(const CSCCLCTDigiCollection *clcts);
  void fillLCTTMBHistos(const CSCCorrelatedLCTDigiCollection *lcts);
  void fillLCTMPCHistos(const CSCCorrelatedLCTDigiCollection *lcts);
  void drawALCTHistos();
  void drawCLCTHistos();
  void drawLCTTMBHistos();
  void drawLCTMPCHistos();

  void bookResolHistos();
  void drawResolHistos();
  void bookEfficHistos();
  void drawEfficHistos();
  void drawHistosForTalks();

  GlobalPoint getGlobalPosition(unsigned int rawId, int keWg, int keyHS) const;
  bool doesALCTCrossCLCT(CSCDetId id, int key_wg, int key_hs) const;
  int getCSCType(const CSCDetId &id);
  double getHsPerRad(const int idh);

  void compare(const CSCALCTDigiCollection *alcts_data,
               const CSCALCTDigiCollection *alcts_emul,
               const CSCCLCTDigiCollection *clcts_data,
               const CSCCLCTDigiCollection *clcts_emul,
               const CSCCLCTPreTriggerDigiCollection *pretrigs_emul,
               const CSCCorrelatedLCTDigiCollection *lcts_data,
               const CSCCorrelatedLCTDigiCollection *lcts_emul,
               const CSCComparatorDigiCollection *compDigis,
               const CSCWireDigiCollection *wireDigis);
  void bookCompHistos();
  void compareALCTs(const CSCALCTDigiCollection *alcts_data,
                    const CSCALCTDigiCollection *alcts_emul,
                    const CSCWireDigiCollection *wireDigis);
  void compareCLCTs(const CSCCLCTDigiCollection *clcts_data,
                    const CSCCLCTDigiCollection *clcts_emul,
                    const CSCCLCTPreTriggerDigiCollection *pretrigs_emul,
                    const CSCComparatorDigiCollection *compDigis);
  void compareLCTs(const CSCCorrelatedLCTDigiCollection *lcts_data,
                   const CSCCorrelatedLCTDigiCollection *lcts_emul,
                   const CSCALCTDigiCollection *alcts_data,
                   const CSCCLCTDigiCollection *clcts_data);
  void compareMPCLCTs(const CSCCorrelatedLCTDigiCollection *lcts_data,
                      const CSCCorrelatedLCTDigiCollection *lcts_emul,
                      const CSCALCTDigiCollection *alcts_data,
                      const CSCCLCTDigiCollection *clcts_data);
  int convertBXofLCT(const int emul_bx,
                     const CSCDetId &detid,
                     const CSCALCTDigiCollection *alcts_data,
                     const CSCCLCTDigiCollection *clcts_data);
  void drawCompHistos();

  void MCStudies(const edm::Event &ev,
                 const reco::GenParticleCollection *genParticles,
                 const edm::PSimHitContainer *allSimHits,
                 const CSCWireDigiCollection *wireDigis,
                 const CSCComparatorDigiCollection *compDigis,
                 const CSCALCTDigiCollection *alcts,
                 const CSCCLCTDigiCollection *clcts);

  void calcResolution(const CSCALCTDigiCollection *alcts,
                      const CSCCLCTDigiCollection *clcts,
                      const CSCWireDigiCollection *wiredc,
                      const CSCComparatorDigiCollection *compdc,
                      const edm::PSimHitContainer *allSimHits);

  void calcEfficiency(const CSCALCTDigiCollection *alcts,
                      const CSCCLCTDigiCollection *clcts,
                      const edm::PSimHitContainer *allSimHits);

  int maxRing(int station);

  template <class T>
  void checkValid(const edm::Handle<T>& h);

  //fill 3 Trees
  MyStubComparison stubs_comparison[4];
  TTree *stub_tree[4];
  TTree *modified_alct;  //tree for modified aclc and llp information
  //fill 3 Trees: alct, clct, lct for data and emul
  TreePerStub perStub[6];
  TTree *event_tree[6];

  // Histograms
  //Hot wires
  TH1F *hHotWire1, *hHotCham1;

  // ALCTs, data
  TH1F *hAlctPerEvent, *hAlctPerChamber, *hAlctPerCSC;
  TH1F *hAlctCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hAlctValid, *hAlctQuality, *hAlctAccel, *hAlctCollis, *hAlctKeyGroup;
  TH1F *hAlctBXN;
  // CLCTs
  TH1F *hClctPerEvent, *hClctPerChamber, *hClctPerCSC;
  TH1F *hClctCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hClctValid, *hClctQuality, *hClctStripType, *hClctSign, *hClctCFEB;
  TH1F *hClctBXN;
  TH1F *hClctKeyStrip[2], *hClctPattern[2];
  TH1F *hClctBendCsc[CSC_TYPES][2], *hClctKeyStripCsc[CSC_TYPES];
  // Correlated LCTs in TMB
  TH1F *hLctTMBPerEvent, *hLctTMBPerChamber;
  TH1F *hLctTMBPerCSC, *hCorrLctTMBPerCSC;
  TH1F *hLctTMBEndcap, *hLctTMBStation, *hLctTMBSector, *hLctTMBRing;
  TH1F *hLctTMBChamber[MAX_STATIONS], *hLctTMBCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hLctTMBValid, *hLctTMBQuality, *hLctTMBKeyGroup;
  TH1F *hLctTMBKeyStrip, *hLctTMBStripType;
  TH1F *hLctTMBPattern, *hLctTMBBend, *hLctTMBBXN;
  // Correlated LCTs in MPC
  TH1F *hLctMPCPerEvent, *hLctMPCPerCSC, *hCorrLctMPCPerCSC;
  TH1F *hLctMPCEndcap, *hLctMPCStation, *hLctMPCSector, *hLctMPCRing;
  TH1F *hLctMPCChamber[MAX_STATIONS];
  TH1F *hLctMPCValid, *hLctMPCQuality, *hLctMPCKeyGroup;
  TH1F *hLctMPCKeyStrip, *hLctMPCStripType;
  TH1F *hLctMPCPattern, *hLctMPCBend, *hLctMPCBXN;

  //Chad's hists

  TH1F *hAlctCompFound;
  TH2F *hAlctCompFound2;
  TH2F *hAlctCompFound2x;
  TH2F *hAlctCompFound2i;

  TH1F *hAlctCompSameN;
  TH2F *hAlctCompSameN2;
  TH2F *hAlctCompSameN2x;
  TH2F *hAlctCompSameN2i;

  TH1F *hAlctCompMatch;
  TH2F *hAlctCompMatch2;
  TH2F *hAlctCompMatch2x;
  TH2F *hAlctCompMatch2i;

  TH1F *hAlctCompTotal;
  TH2F *hAlctCompTotal2;
  TH2F *hAlctCompTotal2x;
  TH2F *hAlctCompTotal2i;

  TH1F *hClctCompFound;
  TH2F *hClctCompFound2;
  TH2F *hClctCompFound2x;
  TH2F *hClctCompFound2i;
  TH1F *hClctCompSameN;
  TH2F *hClctCompSameN2;
  TH2F *hClctCompSameN2x;
  TH2F *hClctCompSameN2i;

  TH1F *hClctCompMatch;
  TH2F *hClctCompMatch2;
  TH2F *hClctCompMatch2x;
  TH2F *hClctCompMatch2i;

  TH1F *hClctCompTotal;
  TH2F *hClctCompTotal2;
  TH2F *hClctCompTotal2x;
  TH2F *hClctCompTotal2i;

  TH1F *hLCTCompFound;
  TH2F *hLCTCompFound2;
  TH2F *hLCTCompFound2x;
  TH2F *hLCTCompFound2i;

  TH1F *hLCTCompSameN;
  TH2F *hLCTCompSameN2;
  TH2F *hLCTCompSameN2x;
  TH2F *hLCTCompSameN2i;

  TH1F *hLCTCompMatch;
  TH2F *hLCTCompMatch2;
  TH2F *hLCTCompMatch2x;
  TH2F *hLCTCompMatch2i;

  TH1F *hLCTCompTotal;
  TH2F *hLCTCompTotal2;
  TH2F *hLCTCompTotal2x;
  TH2F *hLCTCompTotal2i;

  // Histograms for firmware-emulator comparisons
  // ALCTs
  TH1F *hAlctCompFoundCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hAlctCompSameNCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hAlctCompTotalCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hAlctCompMatchCsc[MAX_ENDCAPS][CSC_TYPES];
  // CLCTs
  TH1F *hClctCompFoundCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hClctCompSameNCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hClctCompTotalCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hClctCompMatchCsc[MAX_ENDCAPS][CSC_TYPES];
  // Correlated LCTs
  TH1F *hLctCompFoundCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hLctCompSameNCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hLctCompTotalCsc[MAX_ENDCAPS][CSC_TYPES];
  TH1F *hLctCompMatchCsc[MAX_ENDCAPS][CSC_TYPES];

  // Resolution histograms
  // ALCT
  TH2F *hEtaRecVsSim;
  TH1F *hResolDeltaWG, *hResolDeltaEta;
  TH1F *hAlctVsEta[MAX_STATIONS];
  TH1F *hEtaDiffVsEta[MAX_STATIONS];
  TH1F *hEtaDiffCsc[CSC_TYPES][3];
  TH2F *hEtaDiffVsWireCsc[CSC_TYPES];
  // CLCT
  TH2F *hPhiRecVsSim;
  TH1F *hResolDeltaHS, *hResolDeltaDS;
  TH1F *hResolDeltaPhi, *hResolDeltaPhiHS, *hResolDeltaPhiDS;
  TH1F *hClctVsPhi[MAX_STATIONS];
  TH1F *hPhiDiffVsPhi[MAX_STATIONS];
  TH1F *hPhiDiffCsc[CSC_TYPES][5];
  TH2F *hPhiDiffVsStripCsc[CSC_TYPES][2];
  TH1F *hTrueBendCsc[CSC_TYPES];
  TH1F *hPhiDiffPattern[CSCConstants::NUM_CLCT_PATTERNS];
  // Efficiency histograms
  TH1F *hEfficHitsEta[MAX_STATIONS];
  TH1F *hEfficALCTEta[MAX_STATIONS], *hEfficCLCTEta[MAX_STATIONS];
  TH1F *hEfficHitsEtaCsc[CSC_TYPES];
  TH1F *hEfficALCTEtaCsc[CSC_TYPES], *hEfficCLCTEtaCsc[CSC_TYPES];

  TH1F *hAlctKeyGroupME11;
  TH1F *hClctKeyStripME11;
  TH1F *hLctTMBKeyGroupME11;
  TH1F *hLctTMBKeyStripME11;
  TH1F *hLctMPCKeyGroupME11;
  TH1F *hLctMPCKeyStripME11;

  //modified ALCT Tree variables
  int event;
  int nWireDigis;
  int nALCTs;
  int nStripDigis;
  int nCLCTs;
  int nCLCTsPerChamber[540][10];
  float llp_decay_x[MAXLLPS];
  float llp_decay_y[MAXLLPS];
  float llp_decay_z[MAXLLPS];
  bool llp_in_acceptance[MAXLLPS];
  float llp_eta[MAXLLPS];
  float llp_phi[MAXLLPS];
};

template <class T>
void CSCTriggerPrimitivesReader::checkValid(const edm::Handle<T>& h)
{
  if (!h.isValid()) {
    edm::LogWarning("L1CSCTPEmulatorWrongInput")
      << "+++ Warning: Collection with label " << h.id()
      << " requested, but not found in the event...+++\n";
    return;
  }
}

#endif
