#include <iostream>

#include "Parse/Parser.h"
#include <Parse/DiagnosticsParse.h>

using namespace vc;

void Parser::parseLibraryUnit() {
  switch (Tok.getKind()) {
  case tok::kw_entity:
    parseEntityDecl();
    break;
  case tok::kw_package:
    // Parse def or body
    break;
  case tok::kw_context:
    break;
  case tok::kw_configuration:
    break;
  case tok::kw_architecture:
    parseArchitectureDecl();
    break;
  default:
    DiagnosticBuilder D = Diag->diagnose(diag::unexpected_token);
    D.setLocation(Tok.getLocation());
    D << Tok.getValue();
    consumeToken();
  }
};

void Parser::parseEntityDecl() {
  consumeToken(tok::kw_entity);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();

  if (Tok.isNot(tok::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::kw_is);

  if (Tok.is(tok::kw_generic)) {
    parseGenericClause();
  }

  if (Tok.is(tok::kw_port)) {
    parsePortClause();
  }

parseEntityDecl:
  while (true) {
    break;
  }

  if (Tok.isNot(tok::kw_end)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_decl_or_end);
    D.setLocation(Tok.getLocation());

    // Recover by searching for end keyword or semicolon for new decl
    while (Tok.isNot(tok::kw_end, tok::semicolon, tok::eof)) consumeToken();

    if (Tok.is(tok::eof)) {
      return;
    } else if (Tok.isNot(tok::kw_end)) {
      goto parseEntityDecl;
    }
  }
  
  consumeToken(tok::kw_end);

  consumeIf(tok::kw_entity);

  if (consumeIf(tok::basic_identifier) || consumeIf(tok::extended_identifier)) {
    return;
  }

  if (Tok.isNot(tok::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();
};

void Parser::parseArchitectureDecl() {
  consumeToken(tok::kw_architecture);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();

  if (Tok.isNot(tok::kw_of)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword); 
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::kw_of);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();

  if (Tok.isNot(tok::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::kw_is);

  bool parseDecl = true;
  while (parseDecl) {
    switch(Tok.getKind()) {
      case tok::kw_signal:
        parseSignalDecl();
        break;
      case tok::kw_subtype:
        parseSubtypeDecl();
        break;
      case tok::kw_type:
        parseTypeDecl();
        break;
      default:
        parseDecl = false;
    }
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
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();
};

void Parser::parsePortClause(){

};

void Parser::parseGenericClause(){

};
