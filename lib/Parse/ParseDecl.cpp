#include <iostream>

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "Parse/Parser.h"

using namespace vc;

Decl *Parser::parseSubtypeDecl() {
  consumeToken(tok::kw_subtype);
  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "Expetected identifier" << std::endl;
  }
  consumeToken();

  if (Tok.isNot(tok::kw_is)) {
    std::cout << "Expetected identifier" << std::endl;
  }
  consumeToken();

  parseSubtypeIndication();

  if (Tok.isNot(tok::semicolon)) {
    std::cout << "Expetected ;" << std::endl;
  }
  consumeToken();
  return nullptr;
}

Decl *Parser::parseTypeDecl() {
  consumeToken(tok::kw_type);
  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "Expetected identifier" << std::endl;
  }
  consumeToken();

  if (Tok.is(tok::semicolon)) {
    // incomplete type
    return nullptr;
  }

  if (Tok.isNot(tok::kw_is)) {
    std::cout << "Expetected is keyword" << std::endl;
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
    std::cout << "Expetected ;" << std::endl;
  }
  consumeToken();

  return nullptr;
}

Decl *Parser::parseSignalDecl(bool Interface) {
  //
  consumeIf(tok::kw_signal);

  do {
    if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
      std::cout << "Expetected identifier" << std::endl;
    }
    consumeToken();
  } while (consumeIf(tok::comma));

  if (Tok.isNot(tok::colon)) {
    std::cout << "Expetected colon" << std::endl;
  }
  consumeToken();

  parseSubtypeIndication();

  if (consumeIf(tok::variable_assignment)) {
    parseExpr();
  }

  if (!Interface) {
    if (Tok.isNot(tok::semicolon)) {
      std::cout << "Expetected semicolon" << std::endl;
      return nullptr;
    }
    consumeToken();
  }
  return nullptr;
}