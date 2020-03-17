#ifndef VC_FORMAT_FORMATTOKEN_LEXER_H
#define VC_FORMAT_FORMATTOKEN_LEXER_H

#include <Common/SourceManager.h>
#include <Format/Format.h>
#include <Parse/Lexer.h>

#include <vector>

#include "FormatToken.h"

namespace vc::format {
class FormatTokenLexer {
  Lexer L;
  unsigned TrailingWhitespaces = 0;
  unsigned Column = 0;
  const FormatStyle &Style;

  std::vector<FormatToken*> Tokens;

public:
  FormatTokenLexer(const SourceManager &SManager, SourceFile SFile,
                   const FormatStyle &Style)
      : L(SManager.getStartOfFile(SFile), SManager.getBuffer(SFile)),
        Style(Style) {
    L.setKeepWhitespaces(true);
    L.setKeepComments(true);
  }

  void lex();

  auto getTokens() -> std::vector<FormatToken*> & {
    return Tokens;
  }

private:
  auto getFormatToken() -> FormatToken *;
};
}  // namespace vc::format
#endif