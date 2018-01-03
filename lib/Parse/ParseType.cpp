#include "AST/Type.h"
#include "Parse/Parser.h"
#include <iostream>

using namespace vc;

Type *Parser::parseSubtypeIndication() {
  //
  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "Expetected identifier" << std::endl;
  }
  consumeToken();

  if (Tok.is(tok::left_parenthesis)) {
    parseArrayOrRecordConstr();
  }

  if (Tok.is(tok::kw_range)) {
    parseRangeConstr();
  }

  return nullptr;
}

void Parser::parseRangeConstr(bool allowUnconstrained) {
  consumeToken(tok::kw_range);

  Expr *LHS, *RHS;
  LHS = parseSimpleExpr();

  if (LHS->getKind() != ExprKind::AttrExpr) {
    if (Tok.isNot(tok::kw_to, tok::kw_downto)) {
      std::cout << "expected direction indicator." << std::endl;
      return;
    }
    consumeToken();
    RHS = parseSimpleExpr();
  }
}

void Parser::parseArrayOrRecordConstr() {
  Expr *LHS, *RHS, *R;
  Token State;

  consumeToken(tok::left_parenthesis);
  State = Tok;

  if (Tok.is(tok::kw_open)) {
    consumeToken();
  } else {
    LHS = parseSimpleExpr();
    if (Tok.isNot(tok::kw_to, tok::kw_downto)) {
      if (LHS->getKind() == ExprKind::DeclRefExpr) {
        L->restoreToken(State);
        consumeToken();
        parseRecordConstr();
        return;
      }
      std::cout << "expected direction indication" << std::endl;
      return;
    }
    consumeToken();
    RHS = parseSimpleExpr();
  }
  if (Tok.isNot(tok::right_parenthesis)) {
    std::cout << "expected )" << std::endl;
    return;
  }
  consumeToken();
  if (Tok.is(tok::left_parenthesis)) {
    parseArrayOrRecordConstr();
  }
  return;
}

void Parser::parseRecordConstr() {
  Expr *LHS, *RHS, *R;

  do {
    LHS = parseSimpleName();
    if (Tok.isNot(tok::left_parenthesis)) {
      std::cout << "expected (" << std::endl;
      return;
    }
    parseArrayOrRecordConstr();
  } while (consumeIf(tok::comma));

  if (Tok.isNot(tok::right_parenthesis)) {
    std::cout << "expected )" << std::endl;
    return;
  }
  consumeToken();

  return;
}

void Parser::parseArrayTypeDef() {
  Expr *LHS, *RHS, *R;
  bool IsUnbounded = false;
  bool DoCheck = true;
  consumeToken(tok::kw_array);
  if (Tok.isNot(tok::left_parenthesis)) {
    std::cout << "expected (" << std::endl;
    return;
  }
  consumeToken();

  do {
    LHS = parseSimpleExpr();

    if (DoCheck) {
      DoCheck = false;
      IsUnbounded = Tok.getKind() == tok::kw_range;
    }

    if (IsUnbounded) {
      if (Tok.isNot(tok::kw_range)) {
        std::cout << "expeceted range keyword." << std::endl;
        return;
      }
      consumeToken();
      if (Tok.isNot(tok::box)) {
        std::cout << "expected <>" << std::endl;
        return;
      }
      consumeToken();
    } else {
      if (Tok.isAny(tok::kw_to, tok::kw_downto)) {
        consumeToken();
        RHS = parseSimpleExpr();
      } else {
        if (LHS->getKind() != ExprKind::AttrExpr) {
          std::cout << "expected attribute expression";
        }
      }
    }
  } while (consumeIf(tok::comma));

  if (Tok.isNot(tok::right_parenthesis)) {
    std::cout << "expected )" << std::endl;
    return;
  }
  consumeToken();

  if (Tok.isNot(tok::kw_of)) {
    std::cout << "expected of" << std::endl;
    return;
  }
  consumeToken();
  parseSubtypeIndication();

  if (Tok.isNot(tok::semicolon)) {
    std::cout << "Expetected ;" << std::endl;
  }
  consumeToken();
}

void Parser::parseRecordTypeDef() {}

void Parser::parseEnumTypeDef() {}

void Parser::parseRangeTypeDef() {}