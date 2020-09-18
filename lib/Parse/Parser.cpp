#include "Parse/Parser.h"
#include "Diag/Diagnostic.h"
#include "Parse/DiagnosticsParse.h"

#include <iostream>

using namespace vc;

bool Parser::parseDesignFile() {
  if (Tok.is(TokenKind::NUM_TOKENS))
    consumeToken();

  bool hasDesignUnit = false;
  do {
    hasDesignUnit = parseDesignUnit();
  } while (Tok.isNot(TokenKind::eof));

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
    case TokenKind::kw_library:
      parseLibraryClause();
      break;
    case TokenKind::kw_use:
      parseUseClause();
      break;
    case TokenKind::kw_context:
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
  consumeToken(TokenKind::kw_library);

  while (true) {
    if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
      D.setLocation(Tok.getLocation());
    }
    consumeToken();
    if (consumeIf(TokenKind::semicolon)) {
      break;
    } else if(consumeIf(TokenKind::comma)) {
      continue;
    } else {
      //std::cout << "Expected comma or semicolon";
      //skip
      return;
    }
  }
}

void Parser::parseUseClause() {
  consumeToken(TokenKind::kw_use);
  
  do {
    parseName();
  } while (consumeIf(TokenKind::comma));

  if (Tok.isNot(TokenKind::semicolon)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::semicolon);
}

bool Parser::parseContextReference() { return true; }