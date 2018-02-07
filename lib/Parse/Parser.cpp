#include "Parse/Parser.h"
#include "Diag/Diagnostic.h"
#include "Parse/DiagnosticsParse.h"

#include <iostream>

using namespace vc;

bool Parser::parseDesignFile() {
  if (Tok.is(tok::NUM_TOKENS))
    consumeToken();

  bool hasDesignUnit = false;
  do {
    hasDesignUnit = parseDesignUnit();
  } while (Tok.isNot(tok::eof));

  return hasDesignUnit;
}

bool Parser::parseDesignUnit() {
  parseContextClause();
  parseLibraryUnit();

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
        return;
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
      DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
      D.setLocation(Tok.getLocation());
    }
    consumeToken();
    if (consumeIf(tok::semicolon)) {
      break;
    } else if(consumeIf(tok::comma)) {
      continue;
    } else {
      //std::cout << "Expected comma or semicolon";
      //skip
      return;
    }
  }
}

void Parser::parseUseClause() {
  consumeToken(tok::kw_use);
  
  do {
    parseName();
  } while (consumeIf(tok::comma));

  if (Tok.isNot(tok::semicolon)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::semicolon);
}

bool Parser::parseContextReference() { return true; }