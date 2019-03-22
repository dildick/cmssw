#ifndef Validation_L1T_L1ValidatorHists_h
#define Validation_L1T_L1ValidatorHists_h

#include "TH1F.h"
#include <DataFormats/Candidate/interface/LeafCandidate.h>

#include <string>

#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

class L1ValidatorHists
{
  public:
    L1ValidatorHists();
    ~L1ValidatorHists();

    void Book(DQMStore::IBooker &, std::string dirname);
    void Write();

    int NEvents;

    //enum Type{ IsoEG, NonIsoEG, CenJet, ForJet, TauJet, Muon, Number };
    enum Type{ Egamma, Jet, Tau, Muon, Number };
    std::string Name[4];

    // Each object has gen (pt, eta, phi, pdg), reco (pt, eta, phi, pdg)
    // split by pdg (CenJet, ETM, ETT, ForJet, HTM, HTT, IsoEG, Mu, NoIsoEG, TauJet): 10
    // needs efficiency (by pt, eta), fake rate (by pt, eta), dR (by pt, (eta?))

    MonitorElement *N[Type::Number];

    MonitorElement *Eff_Pt[Type::Number];
    MonitorElement *Eff_Pt_Denom[Type::Number];
    MonitorElement *Eff_Pt_Nomin[Type::Number];
    MonitorElement *Eff_Eta[Type::Number];
    MonitorElement *Eff_Eta_Denom[Type::Number];
    MonitorElement *Eff_Eta_Nomin[Type::Number];
    MonitorElement *TurnOn_15[Type::Number];
    MonitorElement *TurnOn_15_Denom[Type::Number];
    MonitorElement *TurnOn_15_Nomin[Type::Number];
    MonitorElement *TurnOn_30[Type::Number];
    MonitorElement *TurnOn_30_Denom[Type::Number];
    MonitorElement *TurnOn_30_Nomin[Type::Number];
    MonitorElement *dR[Type::Number];
    MonitorElement *dPt[Type::Number];
    MonitorElement *dR_vs_Pt[Type::Number];
    MonitorElement *dPt_vs_Pt[Type::Number];

    void Fill(int, const reco::LeafCandidate *, const reco::LeafCandidate *);
    void FillNumber(int, int);
};


#endif
