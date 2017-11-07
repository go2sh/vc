#include <iostream>

#include "Parse/Parser.h"

using namespace vc;

void Parser::parsePrimaryUnit() {
  switch (Tok.getKind()) {
  case tok::kw_entity:
    parseEntityDecl();
    break;
  case tok::kw_package:
    break;
  case tok::kw_context:
    break;
  case tok::kw_configuration:
    break;
  }
};

void Parser::parseSecondaryUnit() {
  switch (Tok.getKind()) {
  case tok::kw_architecture:
    parseArchitectureDecl();
    break;
  case tok::kw_package:
    break;
  default:
    std::cout << "very bad" << std::endl;
  }
};

void Parser::parseEntityDecl() {
  consumeToken(tok::kw_entity);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "expected identifier" << std::endl;
    return;
  };
  consumeToken();

  if (Tok.isNot(tok::kw_is)) {
    std::cout << "expected is keyword." << std::endl;
    return;
  }
  consumeToken(tok::kw_is);

  if (Tok.is(tok::kw_generic)) {
    parseGenericClause();
  }

  if (Tok.is(tok::kw_port)) {
    parsePortClause();
  }

  while (true) {
    break;
  }

  if (Tok.isNot(tok::kw_end)) {
    std::cout << "decl or  expected end keyword." << std::endl;
    return;
  }
  consumeToken(tok::kw_end);

  consumeIf(tok::kw_entity);

  if (consumeIf(tok::basic_identifier) || consumeIf(tok::extended_identifier)) {
    return;
  }

  if (Tok.isNot(tok::semicolon)) {
    std::cout << "expected semicolon" << std::endl;
    return;
  };
  consumeToken();
};

void Parser::parseArchitectureDecl() {
  consumeToken(tok::kw_architecture);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "expected identifier" << std::endl;
    return;
  };
  consumeToken();

  if (Tok.isNot(tok::kw_of)) {
    std::cout << "expected of keyword." << std::endl;
    return;
  }
  consumeToken(tok::kw_of);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "expected identifier" << std::endl;
    return;
  };
  consumeToken();

  if (Tok.isNot(tok::kw_is)) {
    std::cout << "expected is keyword." << std::endl;
    return;
  }
  consumeToken(tok::kw_is);

  while (true) {
    break;
  }

  if (Tok.isNot(tok::kw_begin)) {
    std::cout << "expected begin keyword." << std::endl;
    return;
  }
  consumeToken(tok::kw_begin);

  if (Tok.isNot(tok::kw_end)) {
    std::cout << "expected begin keyword." << std::endl;
    return;
  }
  consumeToken(tok::kw_end);

  consumeIf(tok::kw_architecture);

  if (consumeIf(tok::basic_identifier) || consumeIf(tok::extended_identifier)) {
    return;
  }

  if (Tok.isNot(tok::semicolon)) {
    std::cout << "expected semicolon" << std::endl;
    return;
  };
  consumeToken();
};

void Parser::parsePortClause(){

};

void Parser::parseGenericClause(){

};
