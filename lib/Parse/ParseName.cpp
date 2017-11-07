#include <iostream>

#include "Parse/Parser.h"

using namespace vc;

void Parser::parseName() {

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    std::cout << "Expected identifier" << std::endl;
    return;
  }
  consumeToken();

  while (true) {
    switch (Tok.getKind()) {
    case tok::dot:
      consumeToken();
      if (Tok.isAny(tok::basic_identifier, tok::extended_identifier)) {

      } else if (Tok.is(tok::kw_all)) {

      } else if (Tok.is(tok::character_literal)) {

      } else if (Tok.is(tok::string_literal)) {

      } else {
        std::cout << "Expectect string,character,identifier,all." << std::endl;
        return;
      }
      consumeToken();
      break;
    default:
      return;
    }
  }
}