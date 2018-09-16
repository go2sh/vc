#ifndef VC_FORMAT_FORMATLINEPARSER_H
#define VC_FORMAT_FORMATLINEPARSER_H

#include "FormatToken.h"
#include <Format/Format.h>

#include <memory>
#include <stack>
#include <vector>

namespace vc {
namespace format {

struct FormatLine {
  /// List of FormatTokens
  std::vector<FormatToken *> Tokens;
  /// List of children
  /// Children lines are lines, that are contained by the block denoted by this
  /// line.
  std::vector<FormatLine *> Children;
  /// Indention Level
  unsigned Level;
};

class FormatLineParser {
  const FormatStyle &Style;

  std::vector<FormatToken *> *Tokens;
  FormatToken *CurrentToken;
  std::unique_ptr<FormatLine> Line;
  std::vector<FormatLine *> Lines;
  std::vector<FormatLine *> *CurrentLines;

  unsigned Level = 0;

  std::vector<FormatToken *>::iterator TokenIt;
  /// Parenthesis stack for parsing constructs with conflicting
  /// parentheses definitions like interfaces or instantiation
  std::vector<FormatToken *> ParenStack;

public:
  FormatLineParser(const FormatStyle &Style, std::vector<FormatToken *> &Tokens)
      : Style(Style), Tokens(&Tokens) {
    Line = std::make_unique<FormatLine>();
    CurrentLines = &Line->Children;
    TokenIt = Tokens.begin();
  }

  void parse();

  friend class FormatLineScopeState;

private:
  void parseDesignFile();
  void parseDesignUnit();

  void parseContextDecl();
  void parseEntityDecl();
  void parseArchitectureDecl();

  void parseInterfaceList();

  FormatToken *nextToken() { return CurrentToken = *TokenIt++; }

  void addToken(FormatToken *Token);
  void addFormatLine();
  bool eof() { return CurrentToken->is(tok::eof); }
};
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMATLINEPARSER_H
