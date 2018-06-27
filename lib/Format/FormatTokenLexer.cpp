#include "FormatTokenLexer.h"
using namespace vc::format;

void FormatTokenLexer::lex(std::vector<FormatToken> &Tokens) {
  do {
    Tokens.push_back(getFormatToken());
  } while(Tokens.back().Tok.isNot(tok::eof));
}

FormatToken FormatTokenLexer::getFormatToken() {
  Token T;
  L.lex(T);

  
}