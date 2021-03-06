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
    case tok::dot:
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
  consumeToken(tok::dot);
  if (Tok.isAny(tok::basic_identifier, tok::extended_identifier)) {
    Suffix = new DeclRefExpr(Tok.getValue());
  } else if (Tok.is(tok::kw_all)) {
    Suffix = new AnyExpr();
  } else if (Tok.is(tok::character_literal)) {
    Suffix = new CharLiteralExpr(Tok.getValue());
  } else if (Tok.is(tok::string_literal)) {
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
  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier); 
    D.setLocation(Tok.getLocation());
    return new ErrorExpr();
  }
  R = new DeclRefExpr(Tok.getValue());
  consumeToken();
  return R;
}