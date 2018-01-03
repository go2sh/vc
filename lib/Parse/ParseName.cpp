#include <iostream>

#include "AST/Expr.h"

#include "Parse/Parser.h"

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
    std::cout << "Expectect string,character,identifier,all." << std::endl;
    Suffix = new ErrorExpr();
  }
  consumeToken();
  return Suffix;
};

Expr *Parser::parseSimpleName() {
  Expr *R;
  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "Expected identifier" << std::endl;
    return new ErrorExpr();
  }
  R = new DeclRefExpr(Tok.getValue());
  consumeToken();
  return R;
}