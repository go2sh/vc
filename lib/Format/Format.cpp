#include <Format/Format.h>

#include "FormatTokenLexer.h"

using namespace vc;

void format::reformat(const SourceManager &SrcMgr, SourceFile SF,
                      const FormatStyle &Style) {
  format::FormatTokenLexer FTLexer(SrcMgr, SF, Style);
  std::vector<FormatToken*> Tokens;

  FTLexer.lex(Tokens);

  

  return;
}