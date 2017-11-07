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
    parseIndexOrRecordConstr();
  }

  if (Tok.is(tok::kw_range)) {
    parseRangeConstr();
  }

  return nullptr;
}

void Parser::parseRangeConstr(bool allowUnconstrained) {
  consumeToken(tok::kw_range);

  Expr *LHS, *RHS;
  if (Tok.isAny(tok::basic_identifier, tok::extended_identifier)) {
    LHS = parseName();
    if (LHS->getKind() != ExprKind::AttrExpr || LHS->getKind() != ExprKind::DeclRefExpr) {
        std::cout << "attribute or identifier expected" << std::endl;
        return;
    }
  } else {
      LHS = parseSimpleExpr();
      if (!LHS->isSimple()) {
          std::cout << "Expecte simple expr" << std::endl;
      }
  }
}

void Parser::parseIndexOrRecordConstr() {}