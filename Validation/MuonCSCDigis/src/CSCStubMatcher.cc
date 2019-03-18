#include "Validation/MuonCSCDigis/interface/CSCStubMatcher.h"

#include <algorithm>

using namespace std;

CSCStubMatcher::CSCStubMatcher(const edm::ParameterSet& pSet, edm::ConsumesCollector && iC)
{
  const auto& cscCLCT = pSet.getParameter<edm::ParameterSet>("cscCLCT");
  minBXCLCT_ = cscCLCT.getParameter<int>("minBX");
  maxBXCLCT_ = cscCLCT.getParameter<int>("maxBX");
  verboseCLCT_ = cscCLCT.getParameter<int>("verbose");
  minNHitsChamberCLCT_ = cscCLCT.getParameter<int>("minNHitsChamber");

  const auto& cscALCT = pSet.getParameter<edm::ParameterSet>("cscALCT");
  minBXALCT_ = cscALCT.getParameter<int>("minBX");
  maxBXALCT_ = cscALCT.getParameter<int>("maxBX");
  verboseALCT_ = cscALCT.getParameter<int>("verbose");
  minNHitsChamberALCT_ = cscALCT.getParameter<int>("minNHitsChamber");

  const auto& cscLCT = pSet.getParameter<edm::ParameterSet>("cscLCT");
  minBXLCT_ = cscLCT.getParameter<int>("minBX");
  maxBXLCT_ = cscLCT.getParameter<int>("maxBX");
  verboseLCT_ = cscLCT.getParameter<int>("verbose");
  minNHitsChamberLCT_ = cscLCT.getParameter<int>("minNHitsChamber");
  hsFromSimHitMean_ = cscLCT.getParameter<bool>("hsFromSimHitMean");

  const auto& cscMPLCT = pSet.getParameter<edm::ParameterSet>("cscMPLCT");
  minBXMPLCT_ = cscMPLCT.getParameter<int>("minBX");
  maxBXMPLCT_ = cscMPLCT.getParameter<int>("maxBX");
  verboseMPLCT_ = cscMPLCT.getParameter<int>("verbose");
  minNHitsChamberMPLCT_ = cscMPLCT.getParameter<int>("minNHitsChamber");

  gemDigiMatcher_.reset(new GEMDigiMatcher(pSet, std::move(iC)));
  cscDigiMatcher_.reset(new CSCDigiMatcher(pSet, std::move(iC)));

  clctToken_ = iC.consumes<CSCCLCTDigiCollection>(cscCLCT.getInputTag("inputTag"));
  alctToken_  = iC.consumes<CSCCLCTDigiCollection>(cscALCT.getInputTag("inputTag"));
  lctToken_  = iC.consumes<CSCCorrelatedLCTDigiCollection>(cscLCT.getInputTag("inputTag"));
  mplctToken_ = iC.consumes<CSCCorrelatedLCTDigiCollection>(cscMPLCT.getInputTag("inputTag"));
}


void CSCDigiMatcher::init(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  gemDigiMatcher_->init(iEvent, iSetup);
  cscDigiMatcher_->init(iEvent, iSetup);

  iEvent.getByToken(clctToken_, clctsH_);
  iEvent.getByToken(alctToken_, alctsH_);
  iEvent.getByToken(lctToken_, lctsH_);
  iEvent.getByToken(mplctToken_, mplctsH_);

/// do the matching
void CSCDigiMatcher::match(const SimTrack& t, const SimVertex& v)
{
  // match simhits first
  gemDigiMatcher_->match(t,v);
  cscDigiMatcher_->match(t,v);

  const CSCCLCTDigiCollection& clcts = *clctsH_.product();
  const CSCALCTDigiCollection& alcts = *alctsH_.product();
  const CSCCorrelatedLCTDigiCollection& lcts = *lctsH_.product();
  const CSCCorrelatedLCTDigiCollection& mplcts = *mplctsH_.product();

  matchCLCTsToSimTrack(clcts);
  matchALCTsToSimTrack(alcts);
  matchLCTsToSimTrack(lcts);
  matchMPLCTsToSimTrack(mplcts);
}

void
CSCStubMatcher::matchCLCTsToSimTrack(const CSCCLCTDigiCollection& clcts)
{
  for (auto detUnitIt= clcts.begin(); detUnitIt!= clcts.end(); ++detUnitIt){
    const CSCDetId& id = (*detUnitIt).first;
    const auto& range =(*detUnitIt).second;
    for (auto digiIt =  range.first; digiIt!=range.second; ++digiIt){
      if (id.station() == 1 and (id.ring() == 1 or id.ring() ==4 ))
        if (verbose()) cout <<"CSCid "<< id <<" CLCTs "<< (*digiIt) << endl;
    }
  }

  const auto& cathode_ids = cscDigiMatcher_->chamberIdsStrip(0);
  int n_minLayers = 0;
  for (const auto& id: cathode_ids)
  {
    CSCDetId ch_id(id);
    if (verbose()){
	cout <<"To check CSC chamber "<< ch_id << endl;
    }
    if (cscDigiMatcher_->nLayersWithStripInChamber(id) >= minNHitsChamberCLCT_) ++n_minLayers;

    // fill 1 half-strip wide gaps
    const auto& digi_strips = cscDigiMatcher_->stripsInChamber(id, 1);
    if (verbose())
    {
      cout<<"clct: digi_strips "<<ch_id<<" Nlayers " << cscDigiMatcher_->nLayersWithStripInChamber(id) <<" ";
      copy(digi_strips.begin(), digi_strips.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
    }

    int ring = ch_id.ring();
    if (ring == 4) ring =1; //use ME1b id to get CLCTs
    CSCDetId ch_id2(ch_id.endcap(), ch_id.station(),  ring, ch_id.chamber(), 0);

    const auto& clcts_in_det = clcts.get(ch_id2);

    for (auto c = clcts_in_det.first; c != clcts_in_det.second; ++c)
    {
      if (verbose()) cout<<"clct "<<ch_id<<" "<<*c<<endl;

      if (!c->isValid()) continue;

      // check that the BX for this stub wasn't too early or too late
      if (c->getBX() < minBXCLCT_ || c->getBX() > maxBXCLCT_) continue;

      int half_strip = c->getKeyStrip() + 1; // CLCT halfstrip numbers start from 0
      if (ch_id.ring() == 4 and ch_id.station() == 1 and half_strip > 128)
        half_strip  = half_strip - 128;

      // store all CLCTs in this chamber
      chamber_to_clcts_all_[id].push_back(*c);

      // match by half-strip with the digis
      if (digi_strips.find(half_strip) == digi_strips.end())
      {
        if (verbose()) cout<<"clctBAD, half_strip "<< half_strip <<endl;
        continue;
      }
      if (verbose()) cout<<"clctGOOD"<<endl;

      // store matching CLCTs in this chamber
      chamber_to_clcts_[id].push_back(*c);
    }
    if (chamber_to_clcts_[id].size() > 2)
    {
      cout<<"WARNING!!! too many CLCTs "<<chamber_to_clcts_[id].size()<<" in "<<ch_id<<endl;
      for (auto &c: chamber_to_clcts_[id]) cout<<"  "<<c<<endl;
    }
  }

  if (verbose() and n_minLayers > 0)
  {
    if (chamber_to_clct_.size() == 0)
    {
      cout<<"effNoCLCT"<<endl;
      for (const auto &it: clcts)
      {
        CSCDetId id(it.first);
        if (useCSCChamberType(id.iChamberType())) continue;
        const auto& clcts_in_det = clcts.get(id);
        for (auto c = clcts_in_det.first; c != clcts_in_det.second; ++c)
        {
          if (!c->isValid()) continue;
          if (verbose()) cout<<" clct: "<<id<<"  "<<*c<<endl;
        }
      }
    }
    else cout<<"effYesCLCT"<<endl;
  }
}


void
CSCStubMatcher::matchALCTsToSimTrack(const CSCALCTDigiCollection& alcts)
{
  const auto& anode_ids = cscDigiMatcher_->chamberIdsWire(0);
  int n_minLayers = 0;
  for (const auto& id: anode_ids)
  {
    if (cscDigiMatcher_->nLayersWithWireInChamber(id) >= minNHitsChamberALCT_) ++n_minLayers;
    CSCDetId ch_id(id);

    // fill 1 WG wide gaps
    const auto& digi_wgs = cscDigiMatcher_->wiregroupsInChamber(id, 1);
    if (verbose())
    {
      cout<<"alct: digi_wgs "<<ch_id<<" ";
      copy(digi_wgs.begin(), digi_wgs.end(), ostream_iterator<int>(cout, " ")); cout<<endl;
    }

    int ring = ch_id.ring();
    if (ring == 4) ring =1; //use ME1b id to get CLCTs
    CSCDetId ch_id2(ch_id.endcap(), ch_id.station(),  ring, ch_id.chamber(), 0);

    const auto& alcts_in_det = alcts.get(ch_id2);
    for (auto a = alcts_in_det.first; a != alcts_in_det.second; ++a)
    {
      if (!a->isValid()) continue;

      if (verbose())
        cout<<"alct "<<ch_id<<" "<<*a<<endl;

      // check that the BX for stub wasn't too early or too late
      if (a->getBX() < minBXALCT_ || a->getBX() > maxBXALCT_) continue;

      int wg = a->getKeyWG() + 1; // as ALCT wiregroups numbers start from 0

      // store all ALCTs in this chamber
      chamber_to_alcts_all_[id].push_back(*a);

      // match by wiregroup with the digis
      if (digi_wgs.find(wg) == digi_wgs.end())
      {
        if (verbose()) cout<<"alctBAD"<<endl;
        continue;
      }
      if (verbose()) cout<<"alctGOOD"<<endl;

      // store matching ALCTs in this chamber
      chamber_to_alcts_[id].push_back(*a);
    }
    if (chamber_to_alcts_[id].size() > 2)
    {
      cout<<"WARNING!!! too many ALCTs "<<chamber_to_alcts_[id].size()<<" in "<<ch_id<<endl;
      for (auto &a: chamber_to_alcts_[id]) cout<<"  "<<a<<endl;
    }
  }

  if (verbose() and n_minLayers > 0)
  {
    if (chamber_to_alcts_.size() == 0)
    {
      cout<<"effNoALCT"<<endl;
      for (const auto &it: alcts)
      {
        CSCDetId id(it.first);
        if (useCSCChamberType(id.iChamberType())) continue;
        const auto& alcts_in_det = alcts.get(id);
        for (auto a = alcts_in_det.first; a != alcts_in_det.second; ++a)
        {
          if (!a->isValid()) continue;
          if (verbose()) cout<<" alct: "<<id<<"  "<<*a<<endl;
        }
      }
    }
    else cout<<"effYesALCT"<<endl;
  }
}


void
CSCStubMatcher::matchLCTsToSimTrack(const CSCCorrelatedLCTDigiCollection& lcts)
{
  // only look for stubs in chambers that already have CLCT and ALCT
  const auto& cathode_ids = chamberIdsAllCLCT(0);
  const auto& anode_ids = chamberIdsAllALCT(0);

  std::set<int> cathode_and_anode_ids;
  std::set_union(
      cathode_ids.begin(), cathode_ids.end(),
      anode_ids.begin(), anode_ids.end(),
      std::inserter(cathode_and_anode_ids, cathode_and_anode_ids.end())
  );

  int n_minLayers = 0;
  for (const auto& id: cathode_and_anode_ids)
    {
    int iLct = -1;
    for (const auto& lct: cscLcts_tmp)
    {
      iLct++;
      bool lct_matched(false);
      bool lct_clct_match(false);
      bool lct_alct_match(false);
      bool lct_gem1_match(false);
      bool lct_gem2_match(false);
      if (verbose()) cout <<"in LCT, getCLCT "<< lct.getCLCT() <<" getALCT "<< lct.getALCT() << endl;
      // Check if matched to an CLCT
      for (const auto& p: clctsInChamber(id)){
        if (p==lct.getCLCT()) {
          lct_clct_match = true;
          if (verbose()) cout<<" LCT matched to CLCT "<< p <<endl;
          break;
        }else{
          if (verbose()) cout<<" LCT Failed to matched to CLCT "<< p <<endl;
        }
      }
      // Check if matched to an ALCT
      for (const auto& p: cscAlctsInChamber(id)){
        //ALCT BX is shifted
        if (p.getKeyWG() == lct.getALCT().getKeyWG()) {
          lct_alct_match = true;
          if (verbose()) cout<<" LCT matched to ALCT "<< p <<endl;
          break;
        }else{
          if (verbose()) cout<<" LCT Failed to matched to ALCT "<< p <<endl;
        }
      }
      // Check if matched to an GEM pad L1
      // fixME here: double check the timing of GEMPad
      if (ch_id.ring()==1 and (ch_id.station()==1 or ch_id.station()==2)) {
        const GEMDetId gemDetIdL1(ch_id.zendcap(),1,ch_id.station(),1,ch_id.chamber(),0);
        for (const auto& p: gemDigiMatcher_->gemPadsInChamber(gemDetIdL1.rawId())){
          if (p==lct.getGEM1()){
            lct_gem1_match = true;
            if (verbose()) cout<<" LCT matched to GEML1 "<< p <<endl;
            break;
          }
        }
        const GEMDetId gemDetIdL2(ch_id.zendcap(),1,ch_id.station(),2,ch_id.chamber(),0);
        // Check if matched to an GEM pad L2
        for (const auto& p: gemDigiMatcher_->gemPadsInChamber(gemDetIdL2.rawId())){
          if (p==lct.getGEM2()){
            lct_gem2_match = true;
            if (verbose()) cout<<" LCT matched to GEML2 "<< p <<endl;
            break;
          }
        }
      }

      lct_matched = ((lct_clct_match and lct_alct_match) or
                     (lct_alct_match and lct_gem1_match and lct_gem2_match) or
                     (lct_clct_match and lct_gem1_match and lct_gem2_match));

      if (chamber_to_lct_.find(id) == chamber_to_lct_.end())   chamber_to_lct_[id] = lcts_tmp[iLct];
      else{
        if (verbose()) cout << "ALARM!!! here already was matching LCT "<<chamber_to_lct_[id]
                            <<" New LCT  "<< lcts_tmp[iLct] <<endl;
      }

      if (lct_matched) {
        if (verbose()) cout<<"this LCT matched to simtrack in chamber "<< ch_id << endl;
        chamber_to_lcts_[id].emplace_back(lcts_tmp[iLct]);
        chamber_to_cscLcts_[id].emplace_back(lct);
      }
    } // lct loop over
  }

  if (verbose() and n_minLayers > 0)
    {
      if (chamber_to_lct_.size() == 0)
        {
          cout<<"No Matched LCT"<<endl;
          for (const auto &it: lcts)
            {
              CSCDetId id(it.first);
              if (useCSCChamberType(id.iChamberType())) continue;
              const auto& lcts_in_det = lcts.get(id);
              for (auto a = lcts_in_det.first; a != lcts_in_det.second; ++a)
                {
                  if (!a->isValid()) continue;
                  if (verbose()) cout<<" lct: "<<id<<"  "<<*a<<endl;
                }
            }

        }
      else cout<<"at least one matched LCT" << std::endl;
    }
}

void
CSCStubMatcher::matchMPLCTsToSimTrack(const CSCCorrelatedLCTDigiCollection& mplcts)
{
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsAllCLCT(int csc_type) const
{
  return selectDetIds(chamber_to_clcts_all_, csc_type);
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsAllALCT(int csc_type) const
{
  return selectDetIds(chamber_to_alcts_all_, csc_type);
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsAllLCT(int csc_type) const
{
  return selectDetIds(chamber_to_lcts_all_, csc_type);
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsAllMPLCT(int csc_type) const
{
  return selectDetIds(chamber_to_mplcts_all_, csc_type);
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsCLCT(int csc_type) const
{
  return selectDetIds(chamber_to_clct_, csc_type);
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsALCT(int csc_type) const
{
  return selectDetIds(chamber_to_alct_, csc_type);
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsLCT(int csc_type) const
{
  return selectDetIds(chamber_to_lct_, csc_type);
}

std::set<unsigned int>
CSCStubMatcher::chamberIdsMPLCT(int csc_type) const
{
  return selectDetIds(chamber_to_mplct_, csc_type);
}

const CSCCLCTDigiContainer&
CSCStubMatcher::allCLCTsInChamber(unsigned int detid) const
{
  if (chamber_to_Clcts_all_.find(detid) == chamber_to_Clcts_all_.end()) return no__clcts_;
  return chamber_to_Clcts_all_.at(detid);
}

const CSCALCTDigiContainer&
CSCStubMatcher::allALCTsInChamber(unsigned int detid) const
{
  if (chamber_to_Alcts_all_.find(detid) == chamber_to_Alcts_all_.end()) return no__alcts_;
  return chamber_to_Alcts_all_.at(detid);
}

const CSCCorrelatedLCTDigiContainer&
CSCStubMatcher::allLCTsInChamber(unsigned int detid) const
{
  if (chamber_to_Lcts_all_.find(detid) == chamber_to_Lcts_all_.end()) return no__lcts_;
  return chamber_to_Lcts_all_.at(detid);
}

const CSCCorrelatedLCTDigiContainer&
CSCStubMatcher::allMPLCTsInChamber(unsigned int detid) const
{
  if (chamber_to_Mplcts_all_.find(detid) == chamber_to_Mplcts_all_.end()) return no__mplcts_;
  return chamber_to_Mplcts_all_.at(detid);
}

const CSCCLCTDigiContainer&
CSCStubMatcher::clctsInChamber(unsigned int detid) const
{
  if (chamber_to_Clcts_.find(detid) == chamber_to_Clcts_.end()) return no__clcts_;
  return chamber_to_Clcts_.at(detid);
}

const CSCALCTDigiContainer&
CSCStubMatcher::alctsInChamber(unsigned int detid) const
{
  if (chamber_to_Alcts_.find(detid) == chamber_to_Alcts_.end()) return no__alcts_;
  return chamber_to_Alcts_.at(detid);
}

const CSCCorrelatedLCTDigiContainer&
CSCStubMatcher::lctsInChamber(unsigned int detid) const
{
  if (chamber_to_Lcts_.find(detid) == chamber_to_Lcts_.end()) return no__lcts_;
  return chamber_to_Lcts_.at(detid);
}

const CSCCorrelatedLCTDigiContainer&
CSCStubMatcher::mplctsInChamber(unsigned int detid) const
{
  if (chamber_to_Mplcts_.find(detid) == chamber_to_Mplcts_.end()) return no__mplcts_;
  return chamber_to_Mplcts_.at(detid);
}

CSCCLCTDigi
CSCStubMatcher::bestClctInChamber(unsigned int detid) const
{
  //sort stubs based on quality
  const auto& input(clctsInChamber(detid));
  int bestQ = 0;
  int index = -1;
  for (unsigned int i=0; i<input.size(); ++i){
    int quality = input[i].getQuality();
    if (quality>bestQ){
      bestQ = quality;
      index = i;
    }
  }
  if (index != -1) return input[index];
  return CSCCLCTDigi();
}

CSCALCTDigi
CSCStubMatcher::bestAlctInChamber(unsigned int detid) const
{
  //sort stubs based on quality
  const auto& input(cscAlctsInChamber(detid));
  int bestQ = 0;
  int index = -1;
  for (unsigned int i=0; i<input.size(); ++i){
    int quality = input[i].getQuality();
    if (quality>bestQ){
      bestQ = quality;
      index = i;
    }
  }
  if (index != -1) return input[index];
  return CSCALCTDigi();
}

CSCCorrelatedLCTDigi
CSCStubMatcher::bestLctInChamber(unsigned int detid) const
{

  //sort stubs based on quality
  const auto& input(cscLctsInChamber(detid));
  int bestQ = 0;
  int index = -1;
  for (unsigned int i=0; i<input.size(); ++i){
    int quality = input[i].getQuality();
    if (quality>bestQ){
      bestQ = quality;
      index = i;
    }
  }
  if (index != -1) return input[index];
  return CSCCorrelatedLCTDigi();
}

float
CSCStubMatcher::zpositionOfLayer(unsigned int detid, int layer) const{

  const auto& id = CSCDetId(detid);
  const auto& chamber(cscGeometry_->chamber(id));
  return fabs(chamber->layer(layer)->centerOfStrip(20).z());
}

int
CSCStubMatcher::nChambersWithCLCT(int min_quality) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsCLCT();
  for (const auto& id: chamber_ids)
  {
    const auto& clct = clctsInChamber(id);
    if (!lct.isValid()) continue;
    if (lct.getQuality() >= min_quality) ++result;
  }
  return result;
}

int
CSCStubMatcher::nChambersWithALCT(int min_quality) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsALCT();
  for (const auto& id: chamber_ids)
  {
    const auto& alct = alctsInChamber(id);
    if (!lct.isValid()) continue;
    if (lct.getQuality() >= min_quality) ++result;
  }
  return result;
}

int
CSCStubMatcher::nChambersWithLCT(int min_quality) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsLCT();
  for (const auto& id: chamber_ids)
  {
    const auto& lct = lctsInChamber(id);
    if (!lct.isValid()) continue;
    if (lct.getQuality() >= min_quality) ++result;
  }
  return result;
}

int
CSCStubMatcher::nChambersWithMPLCT(int min_quality) const
{
  int result = 0;
  const auto& chamber_ids = chamberIdsMPLCT();
  for (const auto& id: chamber_ids)
  {
    const auto& mplct = mplctsInChamber(id);
    if (!lct.isValid()) continue;
    if (lct.getQuality() >= min_quality) ++result;
  }
  return result;
}


bool
CSCStubMatcher::lctInChamber(const CSCDetId& id, const CSCCorrelatedLCTDigi& lct) const
{
  for (const auto& stub: lctsInChamber(id.rawId())){
    if (stub == lct) return true;
  }
  return false;
}


GlobalPoint
CSCStubMatcher::getGlobalPosition(unsigned int rawId, const CSCCorrelatedLCTDigi& lct) const
{
  // taken from https://github.com/cms-sw/cmssw/blob/dc9f78b6af4ad56c9342cf14041b6485a60b0691/L1Trigger/CSCTriggerPrimitives/interface/CSCMotherboardME11GEM.cc
  CSCDetId cscId = CSCDetId(rawId);
  CSCDetId key_id(cscId.endcap(), cscId.station(), cscId.ring(),
                  cscId.chamber(), CSCConstants::KEY_CLCT_LAYER);
  const auto& chamber = getCSCGeometry()->chamber(cscId);
  float fractional_strip = getFractionalStrip(lct);
  const auto& layer_geo = chamber->layer(CSCConstants::KEY_CLCT_LAYER)->geometry();
  // LCT::getKeyWG() also starts from 0
  float wire = layer_geo->middleWireOfGroup(lct.getKeyWG() + 1);
  const LocalPoint& csc_intersect = layer_geo->intersectionOfStripAndWire(fractional_strip, wire);
  const GlobalPoint& csc_gp = cscGeometry_->idToDet(key_id)->surface().toGlobal(csc_intersect);
  return csc_gp;

}


float
CSCStubMatcher::getAverageBendingLCT(unsigned int rawId, const CSCCorrelatedLCTDigi& lct) const
{
  CSCDetId cscId = CSCDetId(rawId);
  float hsWidth(gemvalidation::cscHalfStripWidth(cscId));

  // map the pattern number to min int bending, max int bending
  int maxBendList[CSCConstants::NUM_CLCT_PATTERNS] = {
    0, 11, 11, -11, 9, -9, 7, -7, 5, -5, 3};
  int minBendList[CSCConstants::NUM_CLCT_PATTERNS] = {
    0, 0,   7, -7,  5, -5, 3, -3, 0, 0, -3};

  int averageBend = 0.5 * (minBendList[lct.getPattern()] + maxBendList[lct.getPattern()] );

  // return the average bending
  return averageBend * hsWidth;
}
