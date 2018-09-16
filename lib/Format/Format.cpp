#include <Format/Format.h>

#include "FormatTokenLexer.h"
#include "FormatLineParser.h"

using namespace vc;

void format::reformat(const SourceManager &SrcMgr, SourceFile SF,
                      const FormatStyle &Style) {
  format::FormatTokenLexer FTLexer(SrcMgr, SF, Style);
  std::vector<FormatToken*> Tokens;

  FTLexer.lex(Tokens);

  // Get format lines
  format::FormatLineParser FLParser(Style, Tokens);
  FLParser.parse();

  return;
}