#include "L1Trigger/CSCTriggerPrimitives/interface/CSCComparatorCodeLUT.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

CSCComparatorCodeLUT::CSCComparatorCodeLUT(const std::string& fname)
  : LUT(),
    m_patInWidth(3),
    m_codeInWidth(12) {
  m_totalInWidth = m_patInWidth + m_codeInWidth;
  m_outWidth = 32;

  // m_Mask = (1 << m_patInWidth) - 1;
  // m_qualMask = ((1 << m_codelInWidth) - 1) << m_patInWidth;

  // m_inputs.push_back(MicroGMTConfiguration::QUALITY);
  // m_inputs.push_back(MicroGMTConfiguration::PT);

  if (fname != std::string("")) {
    std::cout << "loaded table " << std::endl;
    load(fname);
  } else {
    initialize();
  }
}

CSCComparatorCodeLUT::CSCComparatorCodeLUT(l1t::LUT* lut)
  : m_totalInWidth(0), m_outWidth(0), m_initialized(true) {
  std::stringstream ss;
  lut->write(ss);
  read(ss);
}

// I/O functions
void CSCComparatorCodeLUT::save(std::ofstream& output) { write(output); }

int CSCComparatorCodeLUT::load(const std::string& inFileName) {
  std::ifstream fstream;
  fstream.open(edm::FileInPath(inFileName.c_str()).fullPath());
  if (!fstream.good()) {
    fstream.close();
    throw cms::Exception("FileOpenError") << "Failed to open LUT file: " << inFileName;
  }
  int readCode = read(fstream);

  m_initialized = true;
  fstream.close();

  return readCode;
}

int CSCComparatorCodeLUT::lookup(int pat, int code) const {
  // normalize these two to the same scale and then calculate?
  if (m_initialized) {
    return lookupPacked(hashInput(checkedInput(pat, m_patInWidth),
                                  checkedInput(code, m_codeInWidth)));
  }
  int result = 0;
  // normalize to out width
  return result;
}

int CSCComparatorCodeLUT::lookupPacked(const int input) const {
  if (m_initialized) {
    std::cout << "lookupPacked " << data((unsigned int)input) << std::endl;
    return data((unsigned int)input);
  }
  throw cms::Exception("Uninitialized") << "If you're not loading a LUT from file you need to implement lookupPacked.";
  return 0;
}

void CSCComparatorCodeLUT::initialize() {
  if (empty()) {
    std::stringstream stream;
    stream << "#<header> V1 " << m_totalInWidth << " " << m_outWidth << " </header> " << std::endl;
    for (int in = 0; in < (1 << m_totalInWidth); ++in) {
      int out = lookupPacked(in);
      stream << in << " " << out << std::endl;
    }
    read(stream);
  }
  m_initialized = true;
}

int CSCComparatorCodeLUT::hashInput(int pat, int code) const {
  int result = 0;
  result += pat << m_codeInWidth;
  result += code;
  std::cout << "result " << result << std::endl;
  return result;
}

// void CSCComparatorCodeLUT::unHashInput(int input, int& pat, int& code) const {
//   pt = input & m_ptRedMask;
//   eta = (input & m_etaRedMask) >> m_ptRedInWidth;
// }

int CSCComparatorCodeLUT::checkedInput(unsigned in, unsigned maxWidth) const {
  unsigned maxIn = (1 << maxWidth) - 1;
  int returnvalue = (in < maxIn ? in : maxIn);
  std::cout << "checkedInput " << in << " " << maxWidth << " " << returnvalue << std::endl;
  return returnvalue;
}
