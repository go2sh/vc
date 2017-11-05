#ifndef VC_PARSER_H
#define VC_PARSER_H

#include "Common/TokenKinds.h"
#include "Parse/Lexer.h"
#include "Parse/Token.h"
#include <cassert>

namespace vc {
class Parser {
public:
  Parser(Lexer *L) : L(L){};

private:
  Lexer *L;
  Token Tok;

public:
  void consumeToken() {
    assert(Tok.isNot(tok::eof) && "Lexing past eof!");
    L->lex(Tok);
  }

  void consumeToken(tok::TokenKind K) {
    assert(Tok.is(K) && "Consuming wrong token kind");
    return consumeToken();
  }

  bool consumeIf(tok::TokenKind K) {
    if (Tok.isNot(K))
      return false;
    consumeToken(K);
    return true;
  }

public:
  bool parseDesignFile();
  bool parseDesignUnit();
  void parseContextClause();

  void parseLibraryClause();
  void parseUseClause();
  bool parseContextReference();

private:
};
}; // namespace vc
#endif