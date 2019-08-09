#ifndef VC_FORMAT_FORMATTOKENINDENTER_H
#define VC_FORMAT_FORMATTOKENINDENTER_H
#include <Format/Format.h>
#include <Format/FormatTypes.h>

#include "EditManager.h"
#include "FormatLineParser.h"
#include "FormatToken.h"

namespace vc {
namespace format {
struct LineState {
  const FormatLine &Line;
  FormatToken *CurrentToken;

  LineState(const FormatLine &Line) : Line(Line) {
    CurrentToken = Line.Tokens.front();
  }

  /// The column index on the current line
  unsigned Column = 0;
};

class FormatTokenIndenter {
  const FormatStyle &Style;
  EditManager &Manager;

public:
  FormatTokenIndenter(const FormatStyle &Style, EditManager &Manager)
      : Style(Style), Manager(Manager) {}

  Penalty addTokenToState(LineState &State, bool Newline);

private:
  Penalty addTokenOnNewLine(LineState &State);
  void addTokenOnCurrentLine(LineState &State);

  void moveStateToNextToken(LineState &State);

  unsigned getNewLineColumn(LineState &State);
};
} // namespace format
} // namespace vc
#endif // !VC_FORMAT_FORMATTOKENINDENTER_H