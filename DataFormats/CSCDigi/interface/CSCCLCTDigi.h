#ifndef CSCDigi_CSCCLCTDigi_h
#define CSCDigi_CSCCLCTDigi_h

/**\class CSCCLCTDigi
 *
 * Digi for CLCT trigger primitives.
 *
 *
 * \author N. Terentiev, CMU
 */

#include <cstdint>
#include <iosfwd>
#include <vector>

class CSCCLCTDigi {
public:

  typedef std::vector<std::vector<uint16_t>> ComparatorContainer;

  /// Constructors
  CSCCLCTDigi(const int valid,
              const int quality,
              const int pattern,
              const int striptype,
              const int bend,
              const int strip,
              const int cfeb,
              const int bx,
              const int trknmb = 0,
              const int fullbx = 0,
              const int compCode = -1);
  /// default
  CSCCLCTDigi();

  /// clear this CLCT
  void clear();

  /// check CLCT validity (1 - valid CLCT)
  bool isValid() const { return valid_; }

  /// set valid
  void setValid(const int valid) { valid_ = valid; }

  /// return quality of a pattern (number of layers hit!)
  int getQuality() const { return quality_; }

  /// set quality
  void setQuality(const int quality) { quality_ = quality; }

  /// return pattern
  int getPattern() const { return pattern_; }

  /// set pattern
  void setPattern(const int pattern) { pattern_ = pattern; }

  /// return striptype
  int getStripType() const { return striptype_; }

  /// set stripType
  void setStripType(const int stripType) { striptype_ = stripType; }

  /// return bend
  int getBend() const { return bend_; }

  /// set bend
  void setBend(const int bend) { bend_ = bend; }

  /// return halfstrip that goes from 0 to 31
  int getStrip() const { return strip_; }

  /// set strip
  void setStrip(const int strip) { strip_ = strip; }

  /// return Key CFEB ID
  int getCFEB() const { return cfeb_; }

  /// set Key CFEB ID
  void setCFEB(const int cfeb) { cfeb_ = cfeb; }

  /// return BX
  int getBX() const { return bx_; }

  /// set bx
  void setBX(const int bx) { bx_ = bx; }

  /// return track number (1,2)
  int getTrknmb() const { return trknmb_; }


  /*Convert strip_ and cfeb_ to keyStrip. Each (D)CFEB has up to
    - 16 strips
    - 32 half-strips
    - 64 quart-strips.
    There are up to 7 (D)CFEBs.

    The "strip_" variable is one of 32 half-strips or 64
    quart-strips on the keylayer of a single (D)CFEB, so that:
    - half-strip  = (cfeb_ * 32 + strip_)
    - quart-strip = (cfeb_ * 64 + strip_).
    Return the corresponding half-strip/quart-strip number
    depending on whether the fit to comparator digis was done.
  */
  int getKeyStrip() const;

  /// Set track number (1,2) after sorting CLCTs.
  void setTrknmb(const uint16_t number) { trknmb_ = number; }

  /// return 12-bit full BX.
  int getFullBX() const { return fullbx_; }

  /// Set 12-bit full BX.
  void setFullBX(const uint16_t fullbx) { fullbx_ = fullbx; }

  // 12-bit comparator code
  int getCompCode() const { return compCode_; }

  void setCompCode(const int16_t code) { compCode_ = code; }

  // comparator hits in this CLCT
  ComparatorContainer getHits() const { return hits_; }

  void setHits(const ComparatorContainer& hits) { hits_ = hits; }

  /// True if the left-hand side has a larger "quality".  Full definition
  /// of "quality" depends on quality word itself, pattern type, and strip
  /// number.
  bool operator>(const CSCCLCTDigi&) const;

  /// True if the two LCTs have exactly the same members (except the number).
  bool operator==(const CSCCLCTDigi&) const;

  /// True if the preceding one is false.
  bool operator!=(const CSCCLCTDigi&) const;

  /// Print content of digi.
  void print() const;

private:
  uint16_t valid_;
  uint16_t quality_;
  uint16_t pattern_;
  uint16_t striptype_;  // not used since mid-2008
  uint16_t bend_;
  uint16_t strip_;
  uint16_t cfeb_;
  uint16_t bx_;
  uint16_t trknmb_;
  uint16_t fullbx_;

  // 12-bit comparator code
  int16_t compCode_;

  // which hits are in this CLCT?
  ComparatorContainer hits_;
};

std::ostream& operator<<(std::ostream& o, const CSCCLCTDigi& digi);

#endif
