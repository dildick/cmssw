#include "L1Trigger/CSCTriggerPrimitives/src/CSCRPCMotherboard.h"
#include "L1Trigger/CSCTriggerPrimitives/src/CSCUpgradeMotherboardLUTGenerator.h"

CSCRPCMotherboard::CSCRPCMotherboard(unsigned endcap, unsigned station,
				     unsigned sector, unsigned subsector,
				     unsigned chamber,
				     const edm::ParameterSet& conf)
  : CSCUpgradeMotherboard(endcap, station, sector, subsector, chamber, conf)
  , allLCTs(match_trig_window_size)
  , maxDeltaBX_(tmbParams_.getParameter<int>("maxDeltaBXRPC"))
  , maxDeltaStrip_(par ? tmbParams_.getParameter<int>("maxDeltaStripRPCEven") : 
		   tmbParams_.getParameter<int>("maxDeltaStripRPCOdd"))
  , useOldLCTDataFormat_(tmbParams_.getParameter<bool>("useOldLCTDataFormat"))
  , dropLowQualityCLCTsNoRPCs_(tmbParams_.getParameter<bool>("dropLowQualityCLCTsNoRPCs"))
  , buildLCTfromALCTandRPC_(tmbParams_.getParameter<bool>("buildLCTfromALCTandRPC"))
  , buildLCTfromCLCTandRPC_(tmbParams_.getParameter<bool>("buildLCTfromCLCTandRPC"))
  , buildLCTfromLowQstubandRPC_(tmbParams_.getParameter<bool>("buildLCTfromLowQstubandRPC"))
  , promoteALCTRPCquality_(tmbParams_.getParameter<bool>("promoteALCTRPCquality"))
  , promoteALCTRPCpattern_(tmbParams_.getParameter<bool>("promoteALCTRPCpattern"))
  , promoteCLCTRPCquality_(tmbParams_.getParameter<bool>("promoteCLCTRPCquality"))
{
}

CSCRPCMotherboard::~CSCRPCMotherboard() 
{
}

void CSCRPCMotherboard::clear()
{
  CSCMotherboard::clear();

  for (int bx = 0; bx < MAX_LCT_BINS; bx++) {
    for (unsigned int mbx = 0; mbx < match_trig_window_size; mbx++) {
      for (int i=0;i<2;i++) {
	allLCTs(bx,mbx,i).clear();
      }
    }
  }
  rpcDigis_.clear();
}

void
CSCRPCMotherboard::run(const CSCWireDigiCollection* wiredc,
		       const CSCComparatorDigiCollection* compdc,
		       const RPCDigiCollection* rpcDigis) 
{
  clear();
  setupGeometry();
  debugLUTs();

  if (rpc_g != nullptr) {
    if (infoV >= 0) edm::LogInfo("L1CSCTPEmulatorSetupInfo")
      << "+++ run() called for RPC-CSC integrated trigger! +++ \n";
    rpcGeometryAvailable = true;
  }

  // check for RE3/1-RE4/1 geometry
  if (rpcGeometryAvailable) {
    if (infoV >= 0) edm::LogInfo("L1CSCTPEmulatorSetupError")
		      << "+++ run() called for RPC-CSC integrated trigger without valid RPC geometry! +++ \n";
    return;
  }

  if (!( alct and clct))
  {
    if (infoV >= 0) edm::LogError("L1CSCTPEmulatorSetupError")
      << "+++ run() called for non-existing ALCT/CLCT processor! +++ \n";
    return;
  }

  alct->setCSCGeometry(csc_g);
  clct->setCSCGeometry(csc_g);

  alctV = alct->run(wiredc); // run anodeLCT
  clctV = clct->run(compdc); // run cathodeLCT

  int used_clct_mask[20];
  for (int c=0;c<20;++c) used_clct_mask[c]=0;

  // retrieve CSCChamber geometry
  const int chid = CSCTriggerNumbering::chamberFromTriggerLabels(theSector, theSubsector, 2, theTrigChamber);
  const CSCDetId csc_id(theEndcap, 2, 1, chid, 0);
  const CSCChamber* cscChamber(csc_g->chamber(csc_id));
  const RPCDetId& rpc_id = CSCUpgradeMotherboardLUTGenerator::Helpers::getRPCfromCSC(csc_id);

  retrieveRPCDigis(rpcDigis, rpc_id.rawId());

  const bool hasRPCDigis(rpcDigis_.size()!=0);

  // ALCT centric matching
  for (int bx_alct = 0; bx_alct < CSCAnodeLCTProcessor::MAX_ALCT_BINS; bx_alct++)
  {
    if (alct->bestALCT[bx_alct].isValid())
    {
      const int bx_clct_start(bx_alct - match_trig_window_size/2);
      const int bx_clct_stop(bx_alct + match_trig_window_size/2);
      if (debug_matching){ 
        LogTrace("CSCRPCMotherboard") << "========================================================================" << std::endl;
        LogTrace("CSCRPCMotherboard") << "ALCT-CLCT matching in ME" << theStation << "/1 chamber: " << csc_id << std::endl;
        LogTrace("CSCRPCMotherboard") << "------------------------------------------------------------------------" << std::endl;
        LogTrace("CSCRPCMotherboard") << "+++ Best ALCT Details: ";
        alct->bestALCT[bx_alct].print();
        LogTrace("CSCRPCMotherboard") << "+++ Second ALCT Details: ";
        alct->secondALCT[bx_alct].print();
        LogTrace("CSCRPCMotherboard") << "------------------------------------------------------------------------" << std::endl;
        LogTrace("CSCRPCMotherboard") << "RPC Chamber " << rpc_id << std::endl;
        printRPCTriggerDigis(bx_clct_start, bx_clct_stop);      
        
        LogTrace("CSCRPCMotherboard") << "------------------------------------------------------------------------" << std::endl;
        LogTrace("CSCRPCMotherboard") << "Attempt ALCT-CLCT matching in ME" << theStation << "/1 in bx range: [" << bx_clct_start << "," << bx_clct_stop << "]" << std::endl;
      }

      // low quality ALCT
      const bool lowQualityALCT(alct->bestALCT[bx_alct].getQuality() == 0);
      
      // ALCT-to-CLCT
      int nSuccesFulMatches = 0;
      for (int bx_clct = bx_clct_start; bx_clct <= bx_clct_stop; bx_clct++) {
        if (bx_clct < 0 or bx_clct >= CSCCathodeLCTProcessor::MAX_CLCT_BINS) continue;          
        if (drop_used_clcts and used_clct_mask[bx_clct]) continue;
        if (clct->bestCLCT[bx_clct].isValid()) {
          
          // pick the digi that corresponds 
          RPCDigiIds matchingDigis;
	  matchingRPCDigis(clct->bestCLCT[bx_clct], clct->secondCLCT[bx_clct],
			   alct->bestALCT[bx_alct], alct->secondALCT[bx_alct], matchingDigis);
          
          // clct quality
          const int quality(clct->bestCLCT[bx_clct].getQuality());
          // low quality ALCT or CLCT
          const bool lowQuality(quality<4 or lowQualityALCT);
          
          if (dropLowQualityCLCTsNoRPCs_ and lowQuality and hasRPCDigis){
            int nFound(matchingDigis.size());
            const bool clctInEdge(clct->bestCLCT[bx_clct].getKeyStrip() < 5 or clct->bestCLCT[bx_clct].getKeyStrip() > 155);
            if (clctInEdge){
              if (debug_matching) LogTrace("CSCRPCMotherboard") << "\tInfo: low quality ALCT or CLCT in CSC chamber edge, don't care about RPC digis" << std::endl;
            }
            else {
              if (nFound != 0){
                if (debug_matching) LogTrace("CSCRPCMotherboard") << "\tInfo: low quality ALCT or CLCT with " << nFound << " matching RPC trigger digis" << std::endl;
              }
              else {
                if (debug_matching) LogTrace("CSCRPCMotherboard") << "\tWarning: low quality ALCT or CLCT without matching RPC trigger digi" << std::endl;
                continue;
              }
            }
          }
          
          int mbx = bx_clct-bx_clct_start;
          correlateLCTsRPC(alct->bestALCT[bx_alct], alct->secondALCT[bx_alct],
			   clct->bestCLCT[bx_clct], clct->secondCLCT[bx_clct], 
			   matchingDigis,
			   allLCTs(bx_alct,mbx,0), allLCTs(bx_alct,mbx,1));            
          ++nSuccesFulMatches;
          if (debug_matching) {
            if (infoV > 1) LogTrace("CSCRPCMotherboard")
			     << "Successful ALCT-CLCT match: bx_clct = " << bx_clct
			     << "; match window: [" << bx_clct_start << "; " << bx_clct_stop
			     << "]; bx_alct = " << bx_alct << std::endl;
	    LogTrace("CSCRPCMotherboard") << "+++ Best CLCT Details: ";
            clct->bestCLCT[bx_clct].print();
            LogTrace("CSCRPCMotherboard") << "+++ Second CLCT Details: ";
            clct->secondCLCT[bx_clct].print();
          }
          if (allLCTs(bx_alct,mbx,0).isValid()) {
            used_clct_mask[bx_clct] += 1;
            if (match_earliest_clct_only) break;
          }
        }
      }
      // ALCT-RPC digi matching
      int nSuccesFulRPCMatches = 0;
      if (nSuccesFulMatches==0 and buildLCTfromALCTandRPC_){
        if (debug_matching) LogTrace("CSCRPCMotherboard") << "++No valid ALCT-CLCT matches in ME"<<theStation<<"1" << std::endl;
        for (int bx_rpc = bx_clct_start; bx_rpc <= bx_clct_stop; bx_rpc++) {
          if (lowQualityALCT and !buildLCTfromLowQstubandRPC_) continue; // build lct from low-Q ALCTs and rpc if para is set true        
          if (not hasRPCDigis) continue;
          
          // find the best matching copad - first one 
          RPCDigiIds digis;
	  matchingRPCDigis(alct->bestALCT[bx_alct], alct->secondALCT[bx_alct], digis);

          if (debug_matching) LogTrace("CSCRPCMotherboard") << "\t++Number of matching RPC Digis in BX " << bx_alct << " : "<< digis.size() << std::endl;
          if (digis.size()==0) continue;
          
	  correlateLCTsRPC(alct->bestALCT[bx_alct], alct->secondALCT[bx_alct], digis,
			   allLCTs(bx_alct,0,0), allLCTs(bx_alct,0,1));            
          if (allLCTs(bx_alct,0,0).isValid()) {
            ++nSuccesFulRPCMatches;            
            if (match_earliest_clct_only) break;
          }
          if (debug_matching) {
            LogTrace("CSCRPCMotherboard") << "Successful ALCT-RPC digi match in ME"<<theStation<<"1: bx_alct = " << bx_alct << std::endl << std::endl;
            LogTrace("CSCRPCMotherboard") << "------------------------------------------------------------------------" << std::endl << std::endl;
          }
        }
      }
    }
    else{
      auto digis(rpcDigis_[bx_alct]);
      if (digis.size() and buildLCTfromCLCTandRPC_) {
        //const int bx_clct_start(bx_alct - match_trig_window_size/2);
        //const int bx_clct_stop(bx_alct + match_trig_window_size/2);
        // RPC-to-CLCT
        int nSuccesFulMatches = 0;
       // for (int bx_clct = bx_clct_start; bx_clct <= bx_clct_stop; bx_clct++)
       // {
        //  if (bx_clct < 0 or bx_clct >= CSCCathodeLCTProcessor::MAX_CLCT_BINS) continue;
          if (drop_used_clcts and used_clct_mask[bx_alct]) continue;
          if (clct->bestCLCT[bx_alct].isValid())
          {          
            if (debug_matching){ 
              LogTrace("CSCRPCMotherboard") << "========================================================================" << std::endl;
              LogTrace("CSCRPCMotherboard") << "RPC-CLCT matching in ME" << theStation << "/1 chamber: " << cscChamber->id() << " in bx: "<<bx_alct<< std::endl;
              LogTrace("CSCRPCMotherboard") << "------------------------------------------------------------------------" << std::endl;
            }
            const int quality(clct->bestCLCT[bx_alct].getQuality());
            // we also use low-Q stubs for the time being
            if (quality < 4 and !buildLCTfromLowQstubandRPC_) continue;
            
            ++nSuccesFulMatches;
            
            int mbx = std::abs(clct->bestCLCT[bx_alct].getBX()-bx_alct);
            int bx_rpc = lct_central_bx;	    
            correlateLCTsRPC(clct->bestCLCT[bx_alct], clct->secondCLCT[bx_alct], digis,
			     allLCTs(bx_rpc,mbx,0), allLCTs(bx_rpc,mbx,1));            
	    if (infoV > 1) {
	      LogTrace("CSCRPCMotherboard") << "Successful RPC-CLCT match in ME"<<theStation<<"/1: bx_alct = " << bx_alct
					    << std::endl;
	      LogTrace("CSCRPCMotherboard") << "+++ Best CLCT Details: ";
	      clct->bestCLCT[bx_alct].print();
              LogTrace("CSCRPCMotherboard") << "+++ Second CLCT Details: ";
              clct->secondCLCT[bx_alct].print();
            }

	    if (allLCTs(bx_rpc,mbx,0).isValid()) {
              used_clct_mask[bx_alct] += 1;
              if (match_earliest_clct_only) break;
            }
          }
      }
    }
  }

  // reduction of nLCTs per each BX
  for (int bx = 0; bx < MAX_LCT_BINS; bx++)
  {
    // counting
    unsigned int n=0;
    for (unsigned int mbx = 0; mbx < match_trig_window_size; mbx++)
      for (int i=0;i<2;i++)
      {
        int cbx = bx + mbx - match_trig_window_size/2;
        if (allLCTs(bx,mbx,i).isValid())
        {
          ++n;
	  if (infoV > 0) LogDebug("CSCMotherboard") 
			   << "LCT"<<i+1<<" "<<bx<<"/"<<cbx<<": "<<allLCTs(bx,mbx,i)<<std::endl;
        }
      }
    
    // some simple cross-bx sorting algorithms
    if (tmb_cross_bx_algo == 1 and (n>2))
    {
      n=0;
      for (unsigned int mbx = 0; mbx < match_trig_window_size; mbx++)
        for (int i=0;i<2;i++)
        {
          if (allLCTs(bx,pref[mbx],i).isValid())
          {
            n++;
            if (n>2) allLCTs(bx,pref[mbx],i).clear();
          }
        }

      n=0;
      for (unsigned int mbx = 0; mbx < match_trig_window_size; mbx++)
        for (int i=0;i<2;i++)
        {
          int cbx = bx + mbx - match_trig_window_size/2;
          if (allLCTs(bx,mbx,i).isValid())
          {
            n++;
            if (infoV > 0) LogDebug("CSCMotherboard") << "LCT"<<i+1<<" "<<bx<<"/"<<cbx<<": "<<allLCTs(bx,mbx,i) << std::endl;
          }
        }
      if (infoV > 0 and n>0) LogDebug("CSCMotherboard") 
        <<"bx "<<bx<<" nnLCT:"<<n<<" "<<n<<std::endl;
    } // x-bx sorting
  }
  
  bool first = true;
  unsigned int n=0;
  for (const auto& p : readoutLCTs()) {
    if (debug_matching and first){
      LogTrace("CSCRPCMotherboard") << "========================================================================" << std::endl;
      LogTrace("CSCRPCMotherboard") << "Counting the final LCTs" << std::endl;
      LogTrace("CSCRPCMotherboard") << "========================================================================" << std::endl;
      first = false;
      LogTrace("CSCRPCMotherboard") << "tmb_cross_bx_algo: " << tmb_cross_bx_algo << std::endl;        
    }
    n++;
    if (debug_matching)
      LogTrace("CSCRPCMotherboard") << "LCT "<<n<<"  " << p <<std::endl;
  }
}



void CSCRPCMotherboard::retrieveRPCDigis(const RPCDigiCollection* rpcDigis, unsigned id)
{
  const auto& chamber(rpc_g->chamber(RPCDetId(id)));
  for (const auto& roll : chamber->rolls()) {
    const RPCDetId& roll_id(roll->id());
    auto digis_in_det = rpcDigis->get(roll_id);
    for (auto digi = digis_in_det.first; digi != digis_in_det.second; ++digi) {
      const int bx_shifted(lct_central_bx + digi->bx());
      for (int bx = bx_shifted - maxDeltaBX_;bx <= bx_shifted + maxDeltaBX_; ++bx) {
        rpcDigis_[bx].emplace_back(roll_id(), *digi);  
      }
    }
  }
}


void CSCRPCMotherboard::printRPCTriggerDigis(int bx_start, int bx_stop)
{
  LogTrace("CSCRPCMotherboard") << "------------------------------------------------------------------------" << std::endl;
  bool first = true;
  for (int bx = bx_start; bx <= bx_stop; bx++) {
    auto& in_strips = rpcDigis_[bx];
    if (first) {
      LogTrace("CSCRPCMotherboard") << "* RPC trigger digis: " << std::endl;
    }
    first = false;
    LogTrace("CSCRPCMotherboard") << "N(digis) BX " << bx << " : " << in_strips.size() << std::endl;
    if (rpcDigis_.size()!=0){
      for (const auto& digi : in_strips){
        auto roll_id(RPCDetId(digi.first));
        LogTrace("CSCRPCMotherboard") << "\tdetId " << digi.first << " " << roll_id << ", digi = " << digi.second.strip() << ", BX = " << digi.second.bx() + 6 << std::endl;
      }
    }
    else
      break;
  }
}


void
CSCRPCMotherboard::matchingRPCDigis(const CSCCLCTDigi& clct, RPCDigiIds& result) const
{
  if (not clct.isValid()) return;
  
  const auto& mymap = (getLUT()->get_csc_hs_to_rpc_strip(par));
  const int lowStrip(mymap[clct.getKeyStrip()].first);
  const int highStrip(mymap[clct.getKeyStrip()].second);
  RPCDigiIdsBX lut = rpcDigis_;
  for (const auto& p: lut[clct.getBX()]){
    auto stripRoll(getStrip(p));
    const int strip_bx = getBX(p.second)+lct_central_bx;
    if (std::abs(clct.getBX()-strip_bx)>1) continue;
    if (std::abs(lowStrip - stripRoll) <= maxDeltaStrip_ or std::abs(stripRoll - highStrip) <= maxDeltaStrip_){
      result.push_back(p);
    }
  }
}


void
CSCRPCMotherboard::matchingRPCDigis(const CSCALCTDigi& alct, RPCDigiIds& result) const
{
  if (not alct.isValid()) return;
  
  const int alctRoll = (getLUT()->get_csc_wg_to_rpc_roll(par))[alct.getKeyWG()];
  RPCDigiIdsBX lut = rpcDigis_;
  for (const auto& p: lut[alct.getBX()]){
    auto stripRoll(getRoll(p));
    if (alctRoll !=  stripRoll) continue;
    result.push_back(p);
  }
}


void 
CSCRPCMotherboard::matchingRPCDigis(const CSCCLCTDigi& clct, 
				    const CSCALCTDigi& alct, 
				    RPCDigiIds& result) const
{
  RPCDigiIds digisClct, digisAlct;

  // digis matching to the CLCT
  matchingRPCDigis(clct, digisClct);
  
  // digis matching to the ALCT
  matchingRPCDigis(alct, digisAlct);

  // collect all *common* digis
  intersection(digisClct, digisAlct, result);
}

void 
CSCRPCMotherboard::matchingRPCDigis(const CSCCLCTDigi& clct1, 
				    const CSCCLCTDigi& clct2, 
				    const CSCALCTDigi& alct1, 
				    const CSCALCTDigi& alct2, 
				    RPCDigiIds& result) const
{
  RPCDigiIds digisClct, digisAlct;

  // digis matching to the CLCTs
  matchingRPCDigis(clct1, clct2, digisClct);

  // digis matching to the ALCTs
  matchingRPCDigis(alct1, alct2, digisAlct);

  // collect *all* matching digis 
  result.reserve(digisClct.size() + digisAlct.size());
  result.insert(std::end(result), std::begin(digisClct), std::end(digisClct));
  result.insert(std::end(result), std::begin(digisAlct), std::end(digisAlct));
}

unsigned int CSCRPCMotherboard::findQualityRPC(const CSCALCTDigi& aLCT, const CSCCLCTDigi& cLCT, bool hasRPC) const
{
  unsigned int quality = 0;
  /*
    Same LCT quality definition as standard LCTs
    a4 and c4 takes RPCs into account!!!
  */
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
      int n_rpc = 0;  
      if (hasRPC) n_rpc = 1;
      const bool a4((aLCT.getQuality() >= 1 and aLCT.getQuality() != 4) or
		    (aLCT.getQuality() == 4 and n_rpc >=1));
      const bool c4((cLCT.getQuality() >= 4) or (cLCT.getQuality() >= 3 and n_rpc>=1));
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

void CSCRPCMotherboard::correlateLCTsRPC(CSCALCTDigi& bestALCT, CSCALCTDigi& secondALCT,
					 CSCCLCTDigi& bestCLCT, CSCCLCTDigi& secondCLCT,
					 const RPCDigiIds& digis,
					 CSCCorrelatedLCTDigi& lct1, CSCCorrelatedLCTDigi& lct2) const
{
  // assume that always anodeBestValid and cathodeBestValid
  if (secondALCT == bestALCT) secondALCT.clear();
  if (secondCLCT == bestCLCT) secondCLCT.clear();
  
  if (digis.size()){
    
    const RPCDigiId& bb_digi = bestMatchingDigi(bestALCT,   bestCLCT,   digis);
    const RPCDigiId& bs_digi = bestMatchingDigi(bestALCT,   secondCLCT, digis);
    const RPCDigiId& sb_digi = bestMatchingDigi(secondALCT, bestCLCT,   digis);
    const RPCDigiId& ss_digi = bestMatchingDigi(secondALCT, secondCLCT, digis);
    
    const bool ok_bb_digi = bestALCT.isValid() and bestCLCT.isValid() and isValid(bb_digi); 
    const bool ok_bs_digi = bestALCT.isValid() and secondCLCT.isValid() and isValid(bs_digi);
    const bool ok_sb_digi = secondALCT.isValid() and bestCLCT.isValid() and isValid(sb_digi);
    const bool ok_ss_digi = secondALCT.isValid() and secondCLCT.isValid() and isValid(ss_digi);
 
    // possible cases with digi
    if (ok_bb_digi or ok_ss_digi){
      if (ok_bb_digi) lct1 = constructLCTsRPC(bestALCT, bestCLCT, bb_digi, 1);
      if (ok_ss_digi) lct2 = constructLCTsRPC(secondALCT, secondCLCT, ss_digi, 2);
    }
    else if(ok_bs_digi or ok_sb_digi){
      if (ok_bs_digi) lct1 = constructLCTsRPC(bestALCT, secondCLCT, bs_digi, 1);
      if (ok_sb_digi) lct2 = constructLCTsRPC(secondALCT, bestCLCT, sb_digi, 2);
    }
    
    // done processing?
    if (lct1.isValid() and lct2.isValid()) return;
    
  } else {
    lct1 = constructLCTs(bestALCT, bestCLCT); 
    lct1.setTrknmb(1);
    
    lct2 = constructLCTs(secondALCT, secondCLCT); 
    lct2.setTrknmb(2);
  }
}

CSCCorrelatedLCTDigi CSCRPCMotherboard::constructLCTsRPC(const CSCCLCTDigi& clct,
							 const RPCDigiId& rpc, int iLCT) const
{
  CSCALCTDigi emptyALCT;
  return constructLCTsRPC(emptyALCT, clct, rpc, iLCT);
}

CSCCorrelatedLCTDigi CSCRPCMotherboard::constructLCTsRPC(const CSCALCTDigi& alct,
							 const RPCDigiId& rpc, int iLCT) const 
{    
  CSCCLCTDigi emptyCLCT;
  return constructLCTsRPC(alct, emptyCLCT, rpc, iLCT);
}

CSCCorrelatedLCTDigi CSCRPCMotherboard::constructLCTsRPC(const CSCALCTDigi& aLCT, 
							 const CSCCLCTDigi& cLCT, 
							 const RPCDigiId& digi, int iLCT) const
{
  // step 1: determine the case
  int lctCase = 0;
  if      (aLCT.isValid() and cLCT.isValid() and isValid(digi))      lctCase = 1;
  else if (aLCT.isValid() and isValid(digi)) lctCase = 2;
  else if (cLCT.isValid() and isValid(digi)) lctCase = 3;

  // step 2: assign properties depending on the LCT dataformat (old/new)
  int pattern = 0, quality = 0, bx = 0, keyStrip = 0, keyWG = 0;
  switch(lctCase){
  case 1: {
    pattern = encodePattern(cLCT.getPattern(), cLCT.getStripType());
    quality = findQualityRPC(aLCT, cLCT, 1);
    bx = aLCT.getBX();
    keyStrip = cLCT.getKeyStrip();
    keyWG = aLCT.getKeyWG();
    break;
  }
  case 2: {
    const auto& mymap1 = getLUT()->get_rpc_strip_to_csc_hs(par);
    pattern = promoteALCTRPCpattern_ ? 10 : 0;
    quality = promoteALCTRPCquality_ ? 15 : 11;
    bx = aLCT.getBX();
    keyStrip = mymap1[getStrip(digi)];
    keyWG = aLCT.getKeyWG();
    break;
  }
  case 3: {
    const auto& mymap2 = getLUT()->get_rpc_roll_to_csc_wg(par);
    pattern = encodePattern(cLCT.getPattern(), cLCT.getStripType());
    quality = promoteCLCTRPCquality_ ? 15 : 11;
    bx = digi.second.bx() + lct_central_bx;;
    keyStrip = cLCT.getKeyStrip();
    // choose the corresponding wire-group in the middle of the partition
    keyWG = 5;//mymap2[digi.roll()];
    break;
  }
  };

  // future work: add a section that produces LCTs according
  // to the new LCT dataformat (not yet defined)

  // step 3: make new LCT with afore assigned properties}
  return CSCCorrelatedLCTDigi(iLCT, 1, quality, keyWG, keyStrip,
			      pattern, 0, bx, 0, 0, 0, theTrigChamber);
  
}

//readout LCTs 
std::vector<CSCCorrelatedLCTDigi> CSCRPCMotherboard::readoutLCTs() const
{
  std::vector<CSCCorrelatedLCTDigi> result;
  allLCTs.getMatched(result);
  if (tmb_cross_bx_algo == 2) CSCUpgradeMotherboard::sortLCTs(result, CSCUpgradeMotherboard::sortLCTsByQuality);
  return result;
}

RPCDigiId CSCRPCMotherboard::bestMatchingDigi(const CSCALCTDigi& alct1, 
					      const matches<RPCDigi>& digis) const
{
  RPCDigiId result;
  // no matching digis for invalid stub
  if (not alct1.isValid()) return result;
  
  // return the first one with the same roll number
  for (const auto& p: digis){
    if (getRoll(p) == getRoll(alct1)){
      return p;
    }
  }
  return result;
}

RPCDigiId CSCRPCMotherboard::bestMatchingDigi(const CSCCLCTDigi& clct, 
					      const matches<RPCDigi>& strips) const
{
  RPCDigiId result;
  // no matching digis for invalid stub
  if (not clct.isValid()) return result;
  
  // return the strip with the smallest bending angle
  float averageStripNumberCSC = getStrip(clct);
  float minDeltaStrip = 999;
  for (const auto& p: strips){
    float averageStripNumberRPC = getStrip(p);
    if (std::abs(averageStripNumberCSC - averageStripNumberRPC) < minDeltaStrip){
      minDeltaStrip = std::abs(averageStripNumberCSC - averageStripNumberRPC);
      result = p;
    }
  }
  return result;
}

RPCDigiId CSCRPCMotherboard::bestMatchingDigi(const CSCALCTDigi& alct1, 
					      const CSCCLCTDigi& clct1, 
					      const matches<RPCDigi>& strips) const
{
  RPCDigiId result;
  // no matching digis for invalid stub
  if (not alct1.isValid() or not clct1.isValid()) return result;
  
  // return the strip with the smallest bending angle
  float averageStripNumberCSC = getStrip(clct1);
  float minDeltaStrip = 999;
  for (const auto& p: strips){
    float averageStripNumberRPC = getStrip(p);
    // add another safety to make sure that the deltaStrip is not larger than max value!!!
    if (std::abs(averageStripNumberCSC - averageStripNumberRPC) < minDeltaStrip and getRoll(p) == getRoll(alct1)){
      minDeltaStrip = std::abs(averageStripNumberCSC - averageStripNumberRPC);
      result = p;
    }
  }
  return result;
}

int CSCRPCMotherboard::getBX(const RPCDigi& p) const
{
  return p.bx();
}

int CSCRPCMotherboard::getRoll(const RPCDigiId& p) const
{
  return (RPCDetId(p.first)).roll();
}

int CSCRPCMotherboard::getRoll(const CSCALCTDigi& alct) const
{
  return (getLUT()->get_csc_wg_to_rpc_roll(par))[alct.getKeyWG()];
}

float CSCRPCMotherboard::getStrip(const RPCDigiId& p) const
{
  return (p.second).strip();
}

float CSCRPCMotherboard::getStrip(const CSCCLCTDigi& clct) const
{
  const auto& mymap = (getLUT()->get_csc_hs_to_rpc_strip(par));
  return 0.5*(mymap[clct.getKeyStrip()].first + mymap[clct.getKeyStrip()].second);
}

bool CSCRPCMotherboard::isValid(const RPCDigiId& p) const
{
  return !(p.second == RPCDigi());
}

void CSCRPCMotherboard::setupGeometry()
{
  CSCUpgradeMotherboard::setupGeometry();
  generator_->setRPCGeometry(rpc_g);
}

template<>
void CSCRPCMotherboard::correlateLCTsRPC<CSCALCTDigi>(const CSCALCTDigi& bestLCT, 
						      const CSCALCTDigi& secondLCT, 
						      const RPCDigiId& bestDigi, 
						      const RPCDigiId& secondDigi,
						      CSCCorrelatedLCTDigi& lct1, 
						      CSCCorrelatedLCTDigi& lct2) const
{
  if ((alct_trig_enable  and bestLCT.isValid()) or
      (match_trig_enable and bestLCT.isValid()))
    {
      lct1 = constructLCTsRPC(bestLCT, bestDigi, 1);
    }
  
  if ((alct_trig_enable  and secondLCT.isValid()) or
      (match_trig_enable and secondLCT.isValid() and secondLCT != bestLCT))
    {
      lct2 = constructLCTsRPC(secondLCT, secondDigi, 2);
    }
}


template<>
void CSCRPCMotherboard::correlateLCTsRPC<CSCCLCTDigi>(const CSCCLCTDigi& bestLCT, 
						      const CSCCLCTDigi& secondLCT,
						      const RPCDigiId& bestDigi, 
						      const RPCDigiId& secondDigi,
						      CSCCorrelatedLCTDigi& lct1, 
						      CSCCorrelatedLCTDigi& lct2) const
{
  if ((clct_trig_enable  and bestLCT.isValid()) or
      (match_trig_enable and bestLCT.isValid()))
    {
      lct1 = constructLCTsRPC(bestLCT, bestDigi, 1);
    }
  
  if ((clct_trig_enable  and secondLCT.isValid()) or
      (match_trig_enable and secondLCT.isValid() and secondLCT != bestLCT))
    {
      lct2 = constructLCTsRPC(secondLCT, secondDigi, 2);
    }
}
