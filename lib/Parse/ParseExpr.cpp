#include <iostream>

#include "AST/Expr.h"
#include "Parse/Parser.h"
#include "Parse/DiagnosticsParse.h"

using namespace vc;

Expr *Parser::parsePrimaryExpr() {
  Expr *R = new Expr(ExprKind::AnyExpr);
  switch (Tok.getKind()) {
  case TokenKind::basic_identifier:
  case TokenKind::extended_identifier:
    R = parseName();
    break;
  case TokenKind::string_literal:
    consumeToken();
    break;
  case TokenKind::character_literal:
    consumeToken();
    break;
  case TokenKind::based_literal:
    consumeToken();
    break;
  case TokenKind::decimal_literal:
    consumeToken();
    break;
  case TokenKind::bit_string_literal:
    consumeToken();
    break;

  case TokenKind::kw_new:
    consumeToken();
    break;

  case TokenKind::left_parenthesis:
    consumeToken();
    R = parseExpr();
    if (Tok.isNot(TokenKind::right_parenthesis)) {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_right_parenthesis);
      D.setLocation(Tok.getLocation());
    }
    consumeToken();
    break;
  default:
    DiagnosticBuilder D = Diag->diagnose(diag::unexpected_token);
    D.setLocation(Tok.getLocation());
  }
  return R;
}

Expr *Parser::parseFactorExpr() {
  Expr *Expr, *LHS, *RHS;

  switch (Tok.getKind()) {
  case TokenKind::kw_abs:
  case TokenKind::kw_not:
  case TokenKind::kw_and:
  case TokenKind::kw_or:
  case TokenKind::kw_nand:
  case TokenKind::kw_nor:
  case TokenKind::kw_xor:
  case TokenKind::kw_xnor:
    // TODO: Operator type
    consumeToken();
    break;
  }
  LHS = parsePrimaryExpr();

  if (false) {

  } else {
    if (Tok.is(TokenKind::double_star)) {
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
  bool hasSign = consumeIf(TokenKind::asterisk) | consumeIf(TokenKind::slash) |
                 consumeIf(TokenKind::kw_mod) | consumeIf(TokenKind::kw_rem);

  LHS = parseFactorExpr();

  // TODO: loop
  if (Tok.isAny(TokenKind::asterisk, TokenKind::slash, TokenKind::kw_mod, TokenKind::kw_rem)) {
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
      consumeIf(TokenKind::plus) | consumeIf(TokenKind::minus) | consumeIf(TokenKind::ampersand);

  LHS = parseTermExpr();

  // TODO: loop
  if (Tok.isAny(TokenKind::plus, TokenKind::minus, TokenKind::ampersand)) {
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
  case TokenKind::kw_sla:
  case TokenKind::kw_sll:
  case TokenKind::kw_sra:
  case TokenKind::kw_srl:
  case TokenKind::kw_rol:
  case TokenKind::kw_ror:
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
  case TokenKind::equal:
  case TokenKind::inequal:
  case TokenKind::greater_equal:
  case TokenKind::greater:
  case TokenKind::less_equal:
  case TokenKind::less:
  case TokenKind::matching_equal:
  case TokenKind::matching_inequal:
  case TokenKind::matching_greater_equal:
  case TokenKind::matching_greater:
  case TokenKind::matching_less_equal:
  case TokenKind::matching_less:
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
  case TokenKind::kw_and:
  case TokenKind::kw_or:
  case TokenKind::kw_nand:
  case TokenKind::kw_nor:
  case TokenKind::kw_xor:
  case TokenKind::kw_xnor:
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

  if (Tok.is(TokenKind::condition_conversion)) {
    consumeToken();
    LHS = parseLogicalExpr();
  } else {
    R = parseLogicalExpr();
  }
  return R;
}