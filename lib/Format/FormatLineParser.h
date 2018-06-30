#ifndef VC_FORMAT_FORMATLINEPARSER_H
#define VC_FORMAT_FORMATLINEPARSER_H

#include <Format/Format.h>
#include <vector>

#include "FormatToken.h"

namespace vc {
namespace format {

struct FormatLine {
  std::vector<FormatToken *> Tokens;
  unsigned Level;
};

class FormatLineParser {
  FormatStyle Style;
  std::vector<FormatToken *> Tokens;

public:
  FormatLineParser(const FormatStyle &Style, std::vector<FormatToken *> &Tokens)
      : Style(Style), Tokens(Tokens) {}

  void parse(std::vector < std::vector<FormatToken *> FormatLines);

private:
  void parseEntity();
};
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMATLINEPARSER_H
