// this class header
#include "L1Trigger/L1TGlobal/interface/MuonShowerCondition.h"

// system include files
#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <algorithm>

// user include files
//   base classes
#include "L1Trigger/L1TGlobal/interface/MuonShowerTemplate.h"
#include "L1Trigger/L1TGlobal/interface/ConditionEvaluation.h"

#include "DataFormats/L1Trigger/interface/MuonShower.h"

#include "L1Trigger/L1TGlobal/interface/GlobalBoard.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/MessageLogger/interface/MessageDrop.h"

// constructors
//     default
l1t::MuonShowerCondition::MuonShowerCondition() : ConditionEvaluation() {
  // empty
}

//     from base template condition (from event setup usually)
l1t::MuonShowerCondition::MuonShowerCondition(const GlobalCondition* muonShowerTemplate,
                                              const GlobalBoard* ptrGTL,
                                              const int nrL1MuShower)
    : ConditionEvaluation(),
      m_gtMuonShowerTemplate(static_cast<const MuonShowerTemplate*>(muonShowerTemplate)),
      m_gtGTL(ptrGTL) {
  m_condMaxNumberObjects = nrL1MuShower;
}

// copy constructor
void l1t::MuonShowerCondition::copy(const l1t::MuonShowerCondition& cp) {
  m_gtMuonShowerTemplate = cp.gtMuonShowerTemplate();
  m_gtGTL = cp.gtGTL();

  m_condMaxNumberObjects = cp.condMaxNumberObjects();
  m_condLastResult = cp.condLastResult();
  m_combinationsInCond = cp.getCombinationsInCond();

  m_verbosity = cp.m_verbosity;
}

l1t::MuonShowerCondition::MuonShowerCondition(const l1t::MuonShowerCondition& cp) : ConditionEvaluation() { copy(cp); }

// destructor
l1t::MuonShowerCondition::~MuonShowerCondition() {
  // empty
}

// equal operator
l1t::MuonShowerCondition& l1t::MuonShowerCondition::operator=(const l1t::MuonShowerCondition& cp) {
  copy(cp);
  return *this;
}

// methods
void l1t::MuonShowerCondition::setGtMuonShowerTemplate(const MuonShowerTemplate* muonTempl) {
  m_gtMuonShowerTemplate = muonTempl;
}

///   set the pointer to GTL
void l1t::MuonShowerCondition::setGtGTL(const GlobalBoard* ptrGTL) { m_gtGTL = ptrGTL; }

// try all object permutations and check spatial correlations, if required
const bool l1t::MuonShowerCondition::evaluateCondition(const int bxEval) const {
  // number of trigger objects in the condition
  int nObjInCond = m_gtMuonShowerTemplate->nrObjects();

  // store the indices of the shower objects
  // from the combination evaluated in the condition
  SingleCombInCond objectsInComb;

  // clear the m_combinationsInCond vector
  (combinationsInCond()).clear();

  // clear the indices in the combination
  objectsInComb.clear();

  // the candidates
  const BXVector<const l1t::MuonShower*>* candVec = m_gtGTL->getCandL1MuShower();

  // Look at objects in bx = bx + relativeBx
  int useBx = bxEval + m_gtMuonShowerTemplate->condRelativeBx();

  // Fail condition if attempting to get Bx outside of range
  if ((useBx < candVec->getFirstBX()) || (useBx > candVec->getLastBX())) {
    return false;
  }

  // If no candidates, no use looking any further.
  int numberObjects = candVec->size(useBx);
  if (numberObjects < nObjInCond) {
    return false;
  }

  std::vector<int> index(numberObjects);

  for (int i = 0; i < numberObjects; ++i) {
    index[i] = i;
  }

  bool condResult = false;

  // index is always zero, as they are global quantities (there is only one object)
  int indexObj = 0;

  bool condGEqVal = m_gtMuonShowerTemplate->condGEq();

  objectsInComb.push_back(indexObj);
  (combinationsInCond()).push_back(objectsInComb);

  // if we get here all checks were successfull for this combination
  // set the general result for evaluateCondition to "true"

  condResult = true;
  return condResult;
}

// load muon candidates
const l1t::MuonShower* l1t::MuonShowerCondition::getCandidate(const int bx, const int indexCand) const {
  return (m_gtGTL->getCandL1MuShower())->at(bx, indexCand);  //BLW Change for BXVector
}

/**
 * checkObjectParameter - Compare a single particle with a numbered condition.
 *
 * @param iCondition The number of the condition.
 * @param cand The candidate to compare.
 *
 * @return The result of the comparison (false if a condition does not exist).
 */

const bool l1t::MuonShowerCondition::checkObjectParameter(const int iCondition,
                                                          const l1t::MuonShower& cand,
                                                          const unsigned int index) const {
  // number of objects in condition
  int nObjInCond = m_gtMuonShowerTemplate->nrObjects();

  if (iCondition >= nObjInCond || iCondition < 0) {
    return false;
  }

  const MuonShowerTemplate::ObjectParameter objPar = (*(m_gtMuonShowerTemplate->objectParameter()))[iCondition];

   LogDebug("L1TGlobal")
     << "\n MuonShowerTemplate::ObjectParameter : " << std::hex
     << "\n\t oneNominalInTime = 0x "
     << objPar.oneNominalInTime << "\n\t oneNominalOutOfTime  = 0x " << objPar.oneNominalOutOfTime
     << "\n\t twoLooseInTime = 0x "
     << objPar.twoLooseInTime << "\n\t twoLooseOutOfTime  = 0x " << objPar.twoLooseOutOfTime
     << std::endl;

   LogDebug("L1TGlobal") << "\n l1t::MuonShower : "
                         << "\n\t oneNominalInTime       = 0x " << cand.isOneNominalInTime()
                         << "\n\t oneNominalOutOfTime       = 0x " << cand.isOneNominalOutOfTime()
                         << "\n\t twoLooseInTime       = 0x " << cand.isTwoLooseInTime()
                         << "\n\t twoLooseOutOfTime       = 0x " << cand.isTwoLooseOutOfTime()
                         << std::dec << std::endl;

   // check oneNominalInTime
   if (cand.isOneNominalInTime() != objPar.oneNominalInTime) {
     LogDebug("L1TGlobal") << "\t\t MuonShower failed oneNominalInTime requirement" << std::endl;
       return false;
   }
   if (cand.isTwoLooseInTime() != objPar.twoLooseInTime) {
     LogDebug("L1TGlobal") << "\t\t MuonShower failed twoLooseInTime requirement" << std::endl;
       return false;
   }
   if (cand.isOneNominalOutOfTime() != objPar.oneNominalOutOfTime) {
     LogDebug("L1TGlobal") << "\t\t MuonShower failed oneNominalOutOfTime requirement" << std::endl;
       return false;
   }
   if (cand.isTwoLooseOutOfTime() != objPar.twoLooseOutOfTime) {
     LogDebug("L1TGlobal") << "\t\t MuonShower failed twoLooseOutOfTime requirement" << std::endl;
       return false;
   }

   //check index
   if (!checkIndex(objPar.indexLow, objPar.indexHigh, index)) {
     LogDebug("L1TGlobal") << "\t\t MuonShower Failed checkIndex " << std::endl;
     return false;
   }

  return true;
}

void l1t::MuonShowerCondition::print(std::ostream& myCout) const {
  m_gtMuonShowerTemplate->print(myCout);

  ConditionEvaluation::print(myCout);
}
