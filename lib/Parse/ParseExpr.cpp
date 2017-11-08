#include <iostream>

#include "AST/Expr.h"
#include "Parse/Parser.h"

using namespace vc;

Expr *Parser::parsePrimaryExpr() {
  Expr *R = new Expr(ExprKind::AnyExpr);
  switch (Tok.getKind()) {
  case tok::basic_identifier:
  case tok::extended_identifier:
    R = parseName();
    break;
  case tok::string_literal:
    consumeToken();
    break;
  case tok::character_literal:
    consumeToken();
    break;
  case tok::based_literal:
    consumeToken();
    break;
  case tok::decimal_literal:
    consumeToken();
    break;
  case tok::bit_string_literal:
    consumeToken();
    break;

  case tok::kw_new:
    consumeToken();
    break;

  case tok::left_parenthesis:
    consumeToken();
    R = parseExpr();
    if (Tok.isNot(tok::right_parenthesis)) {
      std::cout << "expected )" << std::endl;
    }
    consumeToken();
    break;
  default:
    std::cout << "unexpected token" << std::endl;
  }
  return R;
}

Expr *Parser::parseFactorExpr() {
  Expr *Expr, *LHS, *RHS;

  switch (Tok.getKind()) {
  case tok::kw_abs:
  case tok::kw_not:
  case tok::kw_and:
  case tok::kw_or:
  case tok::kw_nand:
  case tok::kw_nor:
  case tok::kw_xor:
  case tok::kw_xnor:
    // TODO: Operator type
    consumeToken();
    break;
  }
  LHS = parsePrimaryExpr();

  if (false) {

  } else {
    if (Tok.is(tok::double_star)) {
      consumeToken();
      RHS = parsePrimaryExpr();
    } else {
      Expr = LHS;
    }
  }

  return Expr;
};

Expr *Parser::parseTermExpr() {
  Expr *LHS, *RHS, *R = new Expr(ExprKind::AnyExpr);
  bool hasSign = consumeIf(tok::asterisk) | consumeIf(tok::slash) |
                 consumeIf(tok::kw_mod) | consumeIf(tok::kw_rem);

  LHS = parseFactorExpr();

  // TODO: loop
  if (Tok.isAny(tok::asterisk, tok::slash, tok::kw_mod, tok::kw_rem)) {
    consumeToken();
    RHS = parseFactorExpr();
  } else {
    R = LHS;
  }

  return R;
}

Expr *Parser::parseSimpleExpr() {
  Expr *LHS, *RHS, *R = new Expr(ExprKind::DeclRefExpr);
  bool hasSign =
      consumeIf(tok::plus) | consumeIf(tok::minus) | consumeIf(tok::ampersand);

  LHS = parseTermExpr();

  // TODO: loop
  if (Tok.isAny(tok::plus, tok::minus, tok::ampersand)) {
    consumeToken();
    RHS = parseTermExpr();
  } else {
    R = LHS;
  }

  return R;
}

Expr *Parser::parseShiftExpr() {
  Expr *LHS, *RHS, *R = new Expr(ExprKind::AnyExpr);

  LHS = parseSimpleExpr();

  switch (Tok.getKind()) {
  case tok::kw_sla:
  case tok::kw_sll:
  case tok::kw_sra:
  case tok::kw_srl:
  case tok::kw_rol:
  case tok::kw_ror:
    consumeToken();
    RHS = parseSimpleExpr();
  default:
    R = LHS;
  }
  return R;
}

Expr *Parser::parseRelationExpr() {
  Expr *LHS, *RHS, *R = new Expr(ExprKind::AnyExpr);

  LHS = parseShiftExpr();

  switch (Tok.getKind()) {
  case tok::equal:
  case tok::inequal:
  case tok::greater_equal:
  case tok::greater:
  case tok::less_equal:
  case tok::less:
  case tok::matching_equal:
  case tok::matching_inequal:
  case tok::matching_greate_equal:
  case tok::matching_greater:
  case tok::matching_less_equal:
  case tok::matching_less:
    consumeToken();
    RHS = parseShiftExpr();
  default:
    R = LHS;
  }
  return R;
}

Expr *Parser::parseLogicalExpr() {
  Expr *LHS, *RHS, *R = new Expr(ExprKind::AnyExpr);

  LHS = parseRelationExpr();

  switch (Tok.getKind()) {
  case tok::kw_and:
  case tok::kw_or:
  case tok::kw_nand:
  case tok::kw_nor:
  case tok::kw_xor:
  case tok::kw_xnor:
    consumeToken();
    RHS = parseRelationExpr();
    break;
  default:
    R = LHS;
  }
  return R;
}

Expr *Parser::parseExpr() {
  Expr *LHS, *R = new Expr(ExprKind::AnyExpr);

  if (Tok.is(tok::condition_conversion)) {
    consumeToken();
    LHS = parseLogicalExpr();
  } else {
    R = parseLogicalExpr();
  }
  return R;
}