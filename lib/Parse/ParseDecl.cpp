#include <iostream>

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "Parse/Parser.h"
#include "Parse/DiagnosticsParse.h"

using namespace vc;

Decl *Parser::parseSubtypeDecl() {
  consumeToken(tok::kw_subtype);
  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  if (Tok.isNot(tok::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  parseSubtypeIndication();

  if (Tok.isNot(tok::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();
  return nullptr;
}

Decl *Parser::parseTypeDecl() {
  consumeToken(tok::kw_type);
  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  if (Tok.is(tok::semicolon)) {
    // incomplete type
    return nullptr;
  }

  if (Tok.isNot(tok::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  switch (Tok.getKind()) {
  case tok::left_parenthesis:
    parseEnumTypeDef();
    break;
  case tok::kw_range:
    parseRangeTypeDef();
    break;
  case tok::kw_array:
    parseArrayTypeDef();
    break;
  case tok::kw_record:
    parseRecordTypeDef();
    break;
  }

  if (Tok.isNot(tok::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  return nullptr;
}

Decl *Parser::parseSignalDecl(bool Interface) {
  //
  consumeIf(tok::kw_signal);

  do {
    if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
      D.setLocation(Tok.getLocation());
    }
    consumeToken();
  } while (consumeIf(tok::comma));

  if (Tok.isNot(tok::colon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_colon);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  parseSubtypeIndication();

  if (consumeIf(tok::variable_assignment)) {
    parseExpr();
  }

  if (!Interface) {
    if (Tok.isNot(tok::semicolon)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
      return nullptr;
    }
    consumeToken();
  }
  return nullptr;
}