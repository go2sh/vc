#ifndef VC_FORMAT_FORMATTOKEN_LEXER_H
#define VC_FORMAT_FORMATTOKEN_LEXER_H

#include <Common/SourceManager.h>
#include <Format/Format.h>
#include <Parse/Lexer.h>

#include <vector>

#include "FormatToken.h"

namespace vc {
namespace format {
class FormatTokenLexer {
  Lexer L;

public:
  FormatTokenLexer(const SourceManager &SManager, SourceFile SFile)
      : L(SManager.getStartOfFile(SFile), SManager.getBuffer(SFile)) {}

  void lex(std::vector<FormatToken> &Tokens);

private:
  FormatToken getFormatToken();
  
};
} // namespace format
} // namespace vc
#endif