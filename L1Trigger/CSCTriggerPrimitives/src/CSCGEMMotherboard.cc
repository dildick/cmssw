#include "L1Trigger/CSCTriggerPrimitives/src/CSCGEMMotherboard.h"
#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"

CSCGEMMotherboard::CSCGEMMotherboard(unsigned endcap, unsigned station,
                                     unsigned sector, unsigned subsector,
                                     unsigned chamber,
                                     const edm::ParameterSet& conf) :
  CSCUpgradeMotherboard(endcap, station, sector, subsector, chamber, conf)
  , maxDeltaBXPad_(tmbParams_.getParameter<int>("maxDeltaBXPad"))
  , maxDeltaBXCoPad_(tmbParams_.getParameter<int>("maxDeltaBXCoPad"))
  , useOldLCTDataFormat_(tmbParams_.getParameter<bool>("useOldLCTDataFormat"))
  , promoteALCTGEMpattern_(tmbParams_.getParameter<bool>("promoteALCTGEMpattern"))
  , promoteALCTGEMquality_(tmbParams_.getParameter<bool>("promoteALCTGEMquality"))
  , doLCTGhostBustingWithGEMs_(tmbParams_.getParameter<bool>("doLCTGhostBustingWithGEMs"))
{
  // super chamber has layer=0!
  gemId = GEMDetId(theRegion, 1, theStation, 0, theChamber, 0).rawId();
  
  const edm::ParameterSet coPadParams(station==1 ? 
				      conf.getParameter<edm::ParameterSet>("copadParamGE11") :
				      conf.getParameter<edm::ParameterSet>("copadParamGE21"));
  coPadProcessor.reset( new GEMCoPadProcessor(endcap, station, chamber, coPadParams) );

  maxDeltaPadL1_ = (par ? tmbParams_.getParameter<int>("maxDeltaPadL1Even") :
		    tmbParams_.getParameter<int>("maxDeltaPadL1Odd") );
  maxDeltaPadL2_ = (par ? tmbParams_.getParameter<int>("maxDeltaPadL2Even") :
		    tmbParams_.getParameter<int>("maxDeltaPadL2Odd") );
}

CSCGEMMotherboard::CSCGEMMotherboard() : CSCUpgradeMotherboard()
{
}

CSCGEMMotherboard::~CSCGEMMotherboard()
{
}

void CSCGEMMotherboard::clear()
{
  pads_.clear();
  coPads_.clear();    
}  


void CSCGEMMotherboard::run(const CSCWireDigiCollection* wiredc,
			    const CSCComparatorDigiCollection* compdc,
			    const GEMPadDigiClusterCollection* gemClusters)
{
  std::unique_ptr<GEMPadDigiCollection> gemPads(new GEMPadDigiCollection());
  coPadProcessor->declusterize(gemClusters, *gemPads);
  run(wiredc, compdc, gemPads.get());
}


void CSCGEMMotherboard::retrieveGEMPads(const GEMPadDigiCollection* gemPads, unsigned id)
{
  pads_.clear();
  auto superChamber(gem_g->superChamber(id));
  for (const auto& ch : superChamber->chambers()) {
    for (const auto& roll : ch->etaPartitions()) {
      GEMDetId roll_id(roll->id());
      auto pads_in_det = gemPads->get(roll_id);
      for (auto pad = pads_in_det.first; pad != pads_in_det.second; ++pad) {
        const int bx_shifted(lct_central_bx + pad->bx());
        for (int bx = bx_shifted - maxDeltaBXPad_;bx <= bx_shifted + maxDeltaBXPad_; ++bx) {
	  pads_[bx].emplace_back(roll_id.rawId(), *pad);  
        }
      }
    }
  }
}

void CSCGEMMotherboard::retrieveGEMCoPads()
{
  coPads_.clear();
  for (const auto& copad: gemCoPadV){
    GEMDetId detId(theRegion, 1, theStation, 0, theChamber, 0);
    coPads_[lct_central_bx + copad.bx(1)].emplace_back(detId.rawId(), copad);
  }
}

CSCCorrelatedLCTDigi CSCGEMMotherboard::constructLCTsGEM(const CSCALCTDigi& alct,
							 const GEMCoPadDigi& gem,
 							 enum CSCPart part,
							 int trknmb) const
{
  return constructLCTsGEM(alct, CSCCLCTDigi(), GEMPadDigi(), gem, part, trknmb);
}


CSCCorrelatedLCTDigi CSCGEMMotherboard::constructLCTsGEM(const CSCCLCTDigi& clct,
							 const GEMCoPadDigi& gem,
							 enum CSCPart part,
							 int trknmb) const
{
  return constructLCTsGEM(CSCALCTDigi(), clct, GEMPadDigi(), gem, part, trknmb);
}

CSCCorrelatedLCTDigi CSCGEMMotherboard::constructLCTsGEM(const CSCALCTDigi& alct,
							 const CSCCLCTDigi& clct,
							 const GEMCoPadDigi& gem,
							 enum CSCPart part,
							 int trknmb) const
{
  return constructLCTsGEM(alct, clct, GEMPadDigi(), gem, part, trknmb);
}


CSCCorrelatedLCTDigi CSCGEMMotherboard::constructLCTsGEM(const CSCALCTDigi& alct,
							 const CSCCLCTDigi& clct,
							 const GEMPadDigi& gem,
							 enum CSCPart part,
							 int trknmb) const
{
  return constructLCTsGEM(alct, clct, gem, GEMCoPadDigi(), part, trknmb);
}

CSCCorrelatedLCTDigi CSCGEMMotherboard::constructLCTsGEM(const CSCALCTDigi& alct,
							 const CSCCLCTDigi& clct,
							 const GEMPadDigi& gem1,
							 const GEMCoPadDigi& gem2,
							 enum CSCPart p, int trknmb) const
{
  // step 1: determine the case
  int lctCase = lctTypes::Invalid;
  if (alct.isValid() and clct.isValid() and gem1.isValid() and not gem2.isValid())      lctCase = lctTypes::ALCTCLCT2GEM;
  else if (alct.isValid() and clct.isValid() and not gem1.isValid() and gem2.isValid()) lctCase = lctTypes::ALCTCLCTGEM;
  else if (alct.isValid() and gem2.isValid()) lctCase = lctTypes::ALCT2GEM;
  else if (clct.isValid() and gem2.isValid()) lctCase = lctTypes::CLCT2GEM;

  // step 2: assign properties depending on the LCT dataformat (old/new)
  int pattern = 0, quality = 0, bx = 0, keyStrip = 0, keyWG = 0;
  switch(lctCase){
  case lctTypes::ALCTCLCTGEM: {
    pattern = encodePattern(clct.getPattern(), clct.getStripType());
    quality = findQualityGEM(alct, clct, 1);
    bx = alct.getBX();
    keyStrip = clct.getKeyStrip();
    keyWG = alct.getKeyWG();
    break;
  }
  case lctTypes::ALCTCLCT2GEM: {
    pattern = encodePattern(clct.getPattern(), clct.getStripType());
    quality = findQualityGEM(alct, clct, 2);
    bx = alct.getBX();
    keyStrip = clct.getKeyStrip();
    keyWG = alct.getKeyWG();
    break;
  }
  case lctTypes::ALCT2GEM: {
    const auto& mymap1 = getLUT()->get_gem_pad_to_csc_hs(par, p);
    pattern = promoteALCTGEMpattern_ ? 10 : 0;
    quality = promoteALCTGEMquality_ ? 15 : 11;
    bx = alct.getBX();
    keyStrip = mymap1[gem2.pad(2)];
    keyWG = alct.getKeyWG();
    break;
  }
  case lctTypes::CLCT2GEM: {
    const auto& mymap2 = getLUT()->get_gem_roll_to_csc_wg(par, p);
    pattern = encodePattern(clct.getPattern(), clct.getStripType());
    quality = promoteCLCTGEMquality_ ? 15 : 11;
    bx = gem2.bx(1) + lct_central_bx;;
    keyStrip = clct.getKeyStrip();
    // choose the corresponding wire-group in the middle of the partition
    keyWG = mymap2[gem2.roll()];
    break;
  }
  };

  // future work: add a section that produces LCTs according
  // to the new LCT dataformat (not yet defined)

  // step 3: make new LCT with afore assigned properties}
  return CSCCorrelatedLCTDigi(trknmb, 1, quality, keyWG, keyStrip,
			      pattern, 0, bx, 0, 0, 0, theTrigChamber);
}


bool CSCGEMMotherboard::isPadInOverlap(int roll)
{
  // this only works for ME1A!
  const auto& mymap = (getLUT()->get_csc_wg_to_gem_roll(par));
  for (unsigned i=0; i<mymap.size(); i++) {
    // overlap region are WGs 10-15
    if ((i < 10) or (i > 15)) continue;
    if ((mymap[i].first <= roll) and (roll <= mymap[i].second)) return true;
  }
  return false;
}

int CSCGEMMotherboard::getBX(const GEMPadDigi& p) const
{
  return p.bx();
}

int CSCGEMMotherboard::getBX(const GEMCoPadDigi& p) const
{
  return p.bx(1);
}

int CSCGEMMotherboard::getRoll(const GEMPadDigiId& p) const
{
  return GEMDetId(p.first).roll();
}

int CSCGEMMotherboard::getRoll(const GEMCoPadDigiId& p) const
{
  return p.second.roll();
}

int CSCGEMMotherboard::getRoll(const CSCALCTDigi& alct) const
{
  return (getLUT()->get_csc_wg_to_gem_roll(par))[alct.getKeyWG()].first;
}

float CSCGEMMotherboard::getPad(const GEMPadDigi& p) const
{
  return p.pad();
}

float CSCGEMMotherboard::getPad(const GEMCoPadDigi& p) const
{
  return 0.5*(p.pad(1) + p.pad(2));
}

float CSCGEMMotherboard::getPad(const CSCCLCTDigi& clct, enum CSCPart part) const
{
  const auto& mymap = (getLUT()->get_csc_hs_to_gem_pad(par, part));
  return 0.5*(mymap[clct.getKeyStrip()].first + mymap[clct.getKeyStrip()].second);
}

void CSCGEMMotherboard::setupGeometry()
{
  CSCUpgradeMotherboard::setupGeometry();
  generator_->setGEMGeometry(gem_g);
}

void CSCGEMMotherboard::printGEMTriggerPads(int bx_start, int bx_stop, enum CSCPart part)
{
  LogTrace("CSCGEMMotherboard") << "------------------------------------------------------------------------" << std::endl;
  LogTrace("CSCGEMMotherboard") << "* GEM trigger pads: " << std::endl;

  for (int bx = bx_start; bx <= bx_stop; bx++) {
    const auto& in_pads = pads_[bx];
    LogTrace("CSCGEMMotherboard") << "N(pads) BX " << bx << " : " << in_pads.size() << std::endl;

    for (const auto& pad : in_pads){ 
      LogTrace("CSCGEMMotherboard") << "\tdetId " << GEMDetId(pad.first) << ", pad = " << pad.second;
      const auto& roll_id(GEMDetId(pad.first));

      if (part==CSCPart::ME11 and isPadInOverlap(GEMDetId(roll_id).roll())) LogTrace("CSCGEMMotherboard") << " (in overlap)" << std::endl;
      else LogTrace("CSCGEMMotherboard") << std::endl;
    }
  }
}


void CSCGEMMotherboard::printGEMTriggerCoPads(int bx_start, int bx_stop, enum CSCPart part)
{
  LogTrace("CSCGEMMotherboard") << "------------------------------------------------------------------------" << std::endl;
  LogTrace("CSCGEMMotherboard") << "* GEM trigger coincidence pads: " << std::endl;

  for (int bx = bx_start; bx <= bx_stop; bx++) {
    const auto& in_pads = coPads_[bx];
    LogTrace("CSCGEMMotherboard") << "N(copads) BX " << bx << " : " << in_pads.size() << std::endl;

    for (const auto& pad : in_pads){ 
      LogTrace("CSCGEMMotherboard") << "\tdetId " << GEMDetId(pad.first) << ", pad = " << pad.second;
      const auto& roll_id(GEMDetId(pad.first));

      if (part==CSCPart::ME11 and isPadInOverlap(GEMDetId(roll_id).roll())) LogTrace("CSCGEMMotherboard") << " (in overlap)" << std::endl;
      else LogTrace("CSCGEMMotherboard") << std::endl;
    }
  }
}


unsigned int CSCGEMMotherboard::findQualityGEM(const CSCALCTDigi& aLCT, const CSCCLCTDigi& cLCT, int gemlayers) const
{
  /*
    Same LCT quality definition as standard LCTs
    a4 and c4 take GEMs into account!!!
  */
  
  unsigned int quality = 0;

  // 2008 definition.
  if (!(aLCT.isValid()) || !(cLCT.isValid())) {
    if (aLCT.isValid() && !(cLCT.isValid()))      quality = 1; // no CLCT
    else if (!(aLCT.isValid()) && cLCT.isValid()) quality = 2; // no ALCT
    else quality = 0; // both absent; should never happen.
  }
  else {
    const int pattern(cLCT.getPattern());
    if (pattern == 1) quality = 3; // layer-trigger in CLCT
    else {
      // ALCT quality is the number of layers hit minus 3.
      // CLCT quality is the number of layers hit.
      bool a4;
      // GE11 
      if (theStation==1) {
	a4 = (aLCT.getQuality() >= 1);
      }
      // GE21
      else if (theStation==2) {
	a4 = (aLCT.getQuality() >= 1) or (aLCT.getQuality() >= 0 and gemlayers >=1);
      }
      else {
	return -1; 
      }
      const bool c4((cLCT.getQuality() >= 4) or (cLCT.getQuality() >= 3 and gemlayers>=1));
      //              quality = 4; "reserved for low-quality muons in future"
      if      (!a4 && !c4) quality = 5; // marginal anode and cathode
      else if ( a4 && !c4) quality = 6; // HQ anode, but marginal cathode
      else if (!a4 &&  c4) quality = 7; // HQ cathode, but marginal anode
      else if ( a4 &&  c4) {
	if (aLCT.getAccelerator()) quality = 8; // HQ muon, but accel ALCT
	else {
	  // quality =  9; "reserved for HQ muons with future patterns
	  // quality = 10; "reserved for HQ muons with future patterns
	  if (pattern == 2 || pattern == 3)      quality = 11;
	  else if (pattern == 4 || pattern == 5) quality = 12;
	  else if (pattern == 6 || pattern == 7) quality = 13;
	  else if (pattern == 8 || pattern == 9) quality = 14;
	  else if (pattern == 10)                quality = 15;
	  else {
	    if (infoV >= 0) edm::LogWarning("L1CSCTPEmulatorWrongValues")
			      << "+++ findQuality: Unexpected CLCT pattern id = "
			      << pattern << "+++\n";
	  }
	}
      }
    }
  }
  return quality;
}

template<>
void CSCGEMMotherboard::matchingPads<GEMPadDigi>(const CSCALCTDigi& alct, enum CSCPart part, matches<GEMPadDigi>& result)
{
  result.clear();
  if (not alct.isValid()) return;

  std::pair<int,int> alctRoll = (getLUT()->CSCGEMMotherboardLUT::get_csc_wg_to_gem_roll(par))[alct.getKeyWG()];
  for (const auto& p: pads_[alct.getBX()]){
    auto padRoll(getRoll(p));
    // only pads in overlap are good for ME1A
    if (part==CSCPart::ME1A and !isPadInOverlap(padRoll)) continue;

    int pad_bx = getBX(p.second)+lct_central_bx;
    if (std::abs(alct.getBX()-pad_bx)>maxDeltaBXPad_) continue;

    if (alctRoll.first == -99 and alctRoll.second == -99) continue;  //invalid region
    else if (alctRoll.first == -99 and !(padRoll <= alctRoll.second)) continue; // top of the chamber
    else if (alctRoll.second == -99 and !(padRoll >= alctRoll.first)) continue; // bottom of the chamber
    else if ((alctRoll.first != -99 and alctRoll.second != -99) and // center
	     (alctRoll.first > padRoll or padRoll > alctRoll.second)) continue;
    result.push_back(p);
  }
}

template<>
void CSCGEMMotherboard::matchingPads<GEMCoPadDigi>(const CSCALCTDigi& alct, enum CSCPart part, matches<GEMCoPadDigi>& result)
{
  result.clear();
  if (not alct.isValid()) return;

  std::pair<int,int> alctRoll = (getLUT()->CSCGEMMotherboardLUT::get_csc_wg_to_gem_roll(par))[alct.getKeyWG()];
  for (const auto& p: coPads_[alct.getBX()]){
    auto padRoll(getRoll(p));
    // only pads in overlap are good for ME1A
    if (part==CSCPart::ME1A and !isPadInOverlap(padRoll)) continue;

    int pad_bx = getBX(p.second)+lct_central_bx;
    if (std::abs(alct.getBX()-pad_bx)>maxDeltaBXCoPad_) continue;

    if (alctRoll.first == -99 and alctRoll.second == -99) continue;  //invalid region
    else if (alctRoll.first == -99 and !(padRoll <= alctRoll.second)) continue; // top of the chamber
    else if (alctRoll.second == -99 and !(padRoll >= alctRoll.first)) continue; // bottom of the chamber
    else if ((alctRoll.first != -99 and alctRoll.second != -99) and // center
	     (alctRoll.first > padRoll or padRoll > alctRoll.second)) continue;
    result.push_back(p);
  }
}

template<>
void CSCGEMMotherboard::matchingPads<GEMPadDigi>(const CSCCLCTDigi& clct, enum CSCPart part, matches<GEMPadDigi>& result)
{
  result.clear();
  if (not clct.isValid()) return;

  const auto& mymap = (getLUT()->get_csc_hs_to_gem_pad(par, part));
  const int lowPad(mymap[clct.getKeyStrip()].first);
  const int highPad(mymap[clct.getKeyStrip()].second);
  for (const auto& p: pads_[clct.getBX()]){
    auto padRoll(getPad(p.second));
    int pad_bx = getBX(p.second)+lct_central_bx;
    if (std::abs(clct.getBX()-pad_bx)>maxDeltaBXPad_) continue;
    if (std::abs(lowPad - padRoll) <= maxDeltaPadL1_ or std::abs(padRoll - highPad) <= maxDeltaPadL1_){
      result.push_back(p);
    }
  }
}

template<>
void CSCGEMMotherboard::matchingPads<GEMCoPadDigi>(const CSCCLCTDigi& clct, enum CSCPart part, matches<GEMCoPadDigi>& result)
{
  result.clear();
  if (not clct.isValid()) return;

  const auto& mymap = (getLUT()->get_csc_hs_to_gem_pad(par, part));
  const int lowPad(mymap[clct.getKeyStrip()].first);
  const int highPad(mymap[clct.getKeyStrip()].second);
  for (const auto& p: coPads_[clct.getBX()]){
    auto padRoll(getPad(p.second));
    int pad_bx = getBX(p.second)+lct_central_bx;
    if (std::abs(clct.getBX()-pad_bx)>maxDeltaBXCoPad_) continue;
    if (std::abs(lowPad - padRoll) <= maxDeltaPadL1_ or std::abs(padRoll - highPad) <= maxDeltaPadL1_){
      result.push_back(p);
    }
  }
}

template<>
void CSCGEMMotherboard::correlateLCTsGEM<CSCALCTDigi>(const CSCALCTDigi& bestLCT, const CSCALCTDigi& secondLCT, 
						      const GEMCoPadDigi& bestCoPad, const GEMCoPadDigi& secondCoPad,
						      CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2, enum CSCPart p)
{
  if ((alct_trig_enable  and bestLCT.isValid()) or
      (match_trig_enable and bestLCT.isValid()))
    {
    lct1 = constructLCTsGEM(bestLCT, bestCoPad, p, 1);
  }
  
  if ((alct_trig_enable  and secondLCT.isValid()) or
      (match_trig_enable and secondLCT.isValid() and secondLCT != bestLCT))
    {
      lct2 = constructLCTsGEM(secondLCT, secondCoPad, p, 2);
    }
}


template<>
void CSCGEMMotherboard::correlateLCTsGEM<CSCCLCTDigi>(const CSCCLCTDigi& bestLCT, const CSCCLCTDigi& secondLCT,
						      const GEMCoPadDigi& bestCoPad, const GEMCoPadDigi& secondCoPad,
						      CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2, enum CSCPart p)
{
  if ((clct_trig_enable  and bestLCT.isValid()) or
      (match_trig_enable and bestLCT.isValid()))
    {
    lct1 = constructLCTsGEM(bestLCT, bestCoPad, p, 1);
  }
  
  if ((clct_trig_enable  and secondLCT.isValid()) or
      (match_trig_enable and secondLCT.isValid() and secondLCT != bestLCT))
    {
      lct2 = constructLCTsGEM(secondLCT, secondCoPad, p, 2);
    }
}
