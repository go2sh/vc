#ifndef VC_FORMAT_FORMAT_CALCULATOR_H
#define VC_FORMAT_FORMAT_CALCULATOR_H
#include <vector>

#include <Format/Format.h>
#include "FormatToken.h"
#include "FormatLineParser.h"

namespace vc {
namespace format {
class FormatCalculator {
  const FormatStyle &Style;

public:
  FormatCalculator(const FormatStyle &Style) : Style(Style) {}


  void calculateFormatInformation(FormatLine &Line);

private:
  bool mustBreakBefore(FormatLine &Line, FormatToken &Token);
  bool canBreakBefore(FormatLine &Line, FormatToken &Token);
  bool spaceRequiredBefore(FormatLine &Line, FormatToken &Token);
  unsigned newLinesBefore(const FormatLine &Line, FormatToken &Token);
};
} // namespace format
} // namespace vc
#endif