#include "AST/Expr.h"
#include "Parse/Parser.h"
#include "Parse/DiagnosticsParse.h"

#include <iostream>

using namespace vc;

Expr *Parser::parseName() {
  Expr* CurrentExpr;
  bool ParseSubExpr = true;

  CurrentExpr = parseSimpleName();

  while (ParseSubExpr) {
    switch (Tok.getKind()) {
    case TokenKind::dot:
      CurrentExpr = new SelectedExpr(CurrentExpr,parseSelectedName());
      break;
    default:
       ParseSubExpr = false;
    }
  }
  return CurrentExpr;
}

Expr *Parser::parseSelectedName() {
  Expr *Suffix;
  consumeToken(TokenKind::dot);
  if (Tok.isAny(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    Suffix = new DeclRefExpr(Tok.getValue());
  } else if (Tok.is(TokenKind::kw_all)) {
    Suffix = new AnyExpr();
  } else if (Tok.is(TokenKind::character_literal)) {
    Suffix = new CharLiteralExpr(Tok.getValue());
  } else if (Tok.is(TokenKind::string_literal)) {
    Suffix = new StringLiteralExpr(Tok.getValue());
  } else {
    DiagnosticBuilder D = Diag->diagnose(diag::wrong_selected_name_suffix); 
    D.setLocation(Tok.getLocation());
    Suffix = new ErrorExpr();
  }
  consumeToken();
  return Suffix;
};

Expr *Parser::parseSimpleName() {
  Expr *R;
  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier); 
    D.setLocation(Tok.getLocation());
    return new ErrorExpr();
  }
  R = new DeclRefExpr(Tok.getValue());
  consumeToken();
  return R;
}