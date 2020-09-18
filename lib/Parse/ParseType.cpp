#include "AST/Type.h"
#include "Parse/Parser.h"
#include "Parse/DiagnosticsParse.h"
#include <iostream>

using namespace vc;

Type *Parser::parseSubtypeIndication() {
  //
  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();

  if (Tok.is(TokenKind::left_parenthesis)) {
    parseArrayOrRecordConstr();
  }

  if (Tok.is(TokenKind::kw_range)) {
    parseRangeConstr();
  }

  //TODO: Error

  return nullptr;
}

void Parser::parseRangeConstr(bool allowUnconstrained) {
  consumeToken(TokenKind::kw_range);

  Expr *LHS, *RHS;
  LHS = parseSimpleExpr();

  if (LHS->getKind() != ExprKind::AttrExpr) {
    if (Tok.isNot(TokenKind::kw_to, TokenKind::kw_downto)) {
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

  consumeToken(TokenKind::left_parenthesis);
  State = Tok;

  if (Tok.is(TokenKind::kw_open)) {
    consumeToken();
  } else {
    LHS = parseSimpleExpr();
    if (Tok.isNot(TokenKind::kw_to, TokenKind::kw_downto)) {
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
  if (Tok.isNot(TokenKind::right_parenthesis)) {
    std::cout << "expected )" << std::endl;
    return;
  }
  consumeToken();
  if (Tok.is(TokenKind::left_parenthesis)) {
    parseArrayOrRecordConstr();
  }
  return;
}

void Parser::parseRecordConstr() {
  Expr *LHS, *RHS, *R;

  do {
    LHS = parseSimpleName();
    if (Tok.isNot(TokenKind::left_parenthesis)) {
      std::cout << "expected (" << std::endl;
      return;
    }
    parseArrayOrRecordConstr();
  } while (consumeIf(TokenKind::comma));

  if (Tok.isNot(TokenKind::right_parenthesis)) {
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
  consumeToken(TokenKind::kw_array);
  if (Tok.isNot(TokenKind::left_parenthesis)) {
    std::cout << "expected (" << std::endl;
    return;
  }
  consumeToken();

  do {
    LHS = parseSimpleExpr();

    if (DoCheck) {
      DoCheck = false;
      IsUnbounded = Tok.getKind() == TokenKind::kw_range;
    }

    if (IsUnbounded) {
      if (Tok.isNot(TokenKind::kw_range)) {
        std::cout << "expeceted range keyword." << std::endl;
        return;
      }
      consumeToken();
      if (Tok.isNot(TokenKind::box)) {
        std::cout << "expected <>" << std::endl;
        return;
      }
      consumeToken();
    } else {
      if (Tok.isAny(TokenKind::kw_to, TokenKind::kw_downto)) {
        consumeToken();
        RHS = parseSimpleExpr();
      } else {
        if (LHS->getKind() != ExprKind::AttrExpr) {
          std::cout << "expected attribute expression";
        }
      }
    }
  } while (consumeIf(TokenKind::comma));

  if (Tok.isNot(TokenKind::right_parenthesis)) {
    std::cout << "expected )" << std::endl;
    return;
  }
  consumeToken();

  if (Tok.isNot(TokenKind::kw_of)) {
    std::cout << "expected of" << std::endl;
    return;
  }
  consumeToken();
  parseSubtypeIndication();

  if (Tok.isNot(TokenKind::semicolon)) {
    std::cout << "Expetected ;" << std::endl;
  }
  consumeToken();
}

void Parser::parseRecordTypeDef() {}

void Parser::parseEnumTypeDef() {}

void Parser::parseRangeTypeDef() {}