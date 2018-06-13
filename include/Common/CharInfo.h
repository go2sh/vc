#ifndef VC_COMMON_CHARINFO_H
#define VC_COMMON_CHARINFO_H
#include <stdint.h>

namespace vc {
namespace charinfo {
extern const uint16_t CharInfo[256];

enum {
  CHAR_HWSPACE = 0x0001,
  CHAR_VWSPACE = 0x0002,
  CHAR_DIGIT = 0x0004,
  CHAR_UPPER = 0x0010,
  CHAR_LOWER = 0x0020,
  CHAR_HEX  = 0x0040,
  CHAR_BITBASE = 0x0080,
  CHAR_BITQUAL = 0x0100,
  CHAR_SPACE = 0x0200,
  CHAR_DELIM = 0x0400,
  CHAR_UNDER = 0x0800,
  CHAR_OTHSPEC = 0x1000
};

enum {
  CHAR_UNDERLINE = CHAR_DELIM | CHAR_UNDER,
  CHAR_BLOWER = CHAR_BITBASE | CHAR_LOWER,
  CHAR_QLOWER = CHAR_BITQUAL | CHAR_LOWER,
  CHAR_XLOWER = CHAR_HEX | CHAR_LOWER,
  CHAR_XBLOWER = CHAR_BITBASE | CHAR_XLOWER,
  CHAR_BUPPER = CHAR_BITBASE | CHAR_UPPER,
  CHAR_QUPPER = CHAR_BITQUAL | CHAR_UPPER,
  CHAR_XUPPER = CHAR_HEX | CHAR_UPPER,
  CHAR_XBUPPER = CHAR_BITBASE | CHAR_XUPPER,
};

enum {
    CHAR_BASIC_GRAPHIC = CHAR_UPPER | CHAR_DIGIT | CHAR_DELIM | CHAR_SPACE,
    CHAR_GRAPHIC = CHAR_BASIC_GRAPHIC | CHAR_LOWER | CHAR_OTHSPEC,
    CHAR_BASIC = CHAR_GRAPHIC | CHAR_HWSPACE | CHAR_VWSPACE,
    CHAR_LETTER = CHAR_UPPER | CHAR_LOWER
};

} // namespace charinfo

inline static bool IsAlphaNumeric(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & (CHAR_DIGIT | CHAR_LETTER)) != 0;
}

inline static bool IsLetterDigitUnderline(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & (CHAR_DIGIT | CHAR_LETTER | CHAR_UNDER)) != 0;
}

inline static bool IsLetter(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & CHAR_LETTER) != 0;
}

inline static bool IsNumeric(uint8_t Char) {
    using namespace charinfo;
    return (CharInfo[Char] & CHAR_DIGIT) != 0;
}

inline static bool IsNumericUnderline(uint8_t Char) {
    using namespace charinfo;
    return (CharInfo[Char] & (CHAR_DIGIT | CHAR_UNDER)) != 0;
}

inline static bool IsBitStringModifier(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & (CHAR_BITBASE | CHAR_BITQUAL)) != 0;
}

inline static bool IsBitStringBase(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & CHAR_BITBASE) != 0;
}

inline static bool IsBitStringQualifier(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & CHAR_BITQUAL) != 0;
}

inline static bool IsGraphicCharacter(uint8_t Char) {
    using namespace charinfo;
    return (CharInfo[Char] & CHAR_GRAPHIC) != 0;
}

inline static bool IsVerticalWhitespace(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & CHAR_VWSPACE) != 0;
}

inline static bool IsHorizontalWhitespace(uint8_t Char) {
  using namespace charinfo;
  return (CharInfo[Char] & CHAR_HWSPACE) != 0;
}

} // namespace vc

#endif