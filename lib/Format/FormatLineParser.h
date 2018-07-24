#ifndef VC_FORMAT_FORMATLINEPARSER_H
#define VC_FORMAT_FORMATLINEPARSER_H

#include <Format/Format.h>
#include <vector>

#include "FormatToken.h"

namespace vc {
namespace format {

struct FormatLine {
  /// List of FormatTokens
  std::vector<FormatToken *> Tokens;
  /// Indention Level
  unsigned Level;
};

class FormatLineParser {
  FormatStyle Style;
  std::unique_ptr<FormatLine> Line;

  std::vector<FormatToken *> Tokens;
  std::vector<FormatLine *> Lines;

  unsigned Level = 0;

public:
  FormatLineParser(const FormatStyle &Style, std::vector<FormatToken *> &Tokens)
      : Style(Style), Tokens(Tokens) {}

  void parse();

private:
  void parseDesignFile();
  void parseContextDecl();
  void parseEntityDecl();

  bool eof() {
    
  }
};
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMATLINEPARSER_H
