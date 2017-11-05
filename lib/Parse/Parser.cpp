#include "Parse/Parser.h"

#include <iostream>

using namespace vc;

bool Parser::parseDesignFile() {
  if (Tok.is(tok::NUM_TOKENS))
    consumeToken();

  bool hasDesignUnit = false;
  do {
    hasDesignUnit = parseDesignUnit();
  } while (hasDesignUnit);

  return hasDesignUnit;
}

bool Parser::parseDesignUnit() {
  parseContextClause();

  return false;
}

void Parser::parseContextClause() {
  Token CurrentTok = Tok;

  while (true) {
    switch (Tok.getKind()) {
    case tok::kw_library:
      parseLibraryClause();
      break;
    case tok::kw_use:
      parseUseClause();
      break;
    case tok::kw_context:
      if (!parseContextReference()) {
        L->restoreToken(CurrentTok);
        consumeToken();
      }
      break;
    default:
      return;
    }
  }
}

void Parser::parseLibraryClause() {
  consumeToken(tok::kw_library);

  while (true) {
    if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
      std::cout << "Expected identifier.";
      // skip;
    }
    consumeToken();
    if (Tok.is(tok::semicolon)) {
      consumeToken();
      break;
    } else if(Tok.is(tok::comma)) {
      continue;
    } else {
      std::cout << "Expected comma or semicolon";
      //skip
      return;
    }
  }
}

void Parser::parseUseClause() {}

bool Parser::parseContextReference() { return true; }