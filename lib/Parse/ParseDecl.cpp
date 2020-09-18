#include <iostream>

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "Parse/Parser.h"
#include "Parse/DiagnosticsParse.h"

using namespace vc;

Decl *Parser::parseSubtypeDecl() {
  consumeToken(TokenKind::kw_subtype);
  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  if (Tok.isNot(TokenKind::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  parseSubtypeIndication();

  if (Tok.isNot(TokenKind::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();
  return nullptr;
}

Decl *Parser::parseTypeDecl() {
  consumeToken(TokenKind::kw_type);
  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  if (Tok.is(TokenKind::semicolon)) {
    // incomplete type
    return nullptr;
  }

  if (Tok.isNot(TokenKind::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  switch (Tok.getKind()) {
  case TokenKind::left_parenthesis:
    parseEnumTypeDef();
    break;
  case TokenKind::kw_range:
    parseRangeTypeDef();
    break;
  case TokenKind::kw_array:
    parseArrayTypeDef();
    break;
  case TokenKind::kw_record:
    parseRecordTypeDef();
    break;
  }

  if (Tok.isNot(TokenKind::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  return nullptr;
}

Decl *Parser::parseSignalDecl(bool Interface) {
  //
  consumeIf(TokenKind::kw_signal);

  do {
    if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
      D.setLocation(Tok.getLocation());
    }
    consumeToken();
  } while (consumeIf(TokenKind::comma));

  if (Tok.isNot(TokenKind::colon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_colon);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  parseSubtypeIndication();

  if (consumeIf(TokenKind::variable_assignment)) {
    parseExpr();
  }

  if (!Interface) {
    if (Tok.isNot(TokenKind::semicolon)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
      return nullptr;
    }
    consumeToken();
  }
  return nullptr;
}