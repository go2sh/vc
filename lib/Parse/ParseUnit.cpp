#include <iostream>

#include "Parse/Parser.h"
#include <Parse/DiagnosticsParse.h>

using namespace vc;

void Parser::parseLibraryUnit() {
  switch (Tok.getKind()) {
  case TokenKind::kw_entity:
    parseEntityDecl();
    break;
  case TokenKind::kw_package:
    // Parse def or body
    break;
  case TokenKind::kw_context:
    break;
  case TokenKind::kw_configuration:
    break;
  case TokenKind::kw_architecture:
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
  consumeToken(TokenKind::kw_entity);

  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();

  if (Tok.isNot(TokenKind::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::kw_is);

  if (Tok.is(TokenKind::kw_generic)) {
    parseGenericClause();
  }

  if (Tok.is(TokenKind::kw_port)) {
    parsePortClause();
  }

parseEntityDecl:
  while (true) {
    break;
  }

  if (Tok.isNot(TokenKind::kw_end)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_decl_or_end);
    D.setLocation(Tok.getLocation());

    // Recover by searching for end keyword or semicolon for new decl
    while (Tok.isNot(TokenKind::kw_end, TokenKind::semicolon, TokenKind::eof))
      consumeToken();

    if (Tok.is(TokenKind::eof)) {
      return;
    } else if (Tok.isNot(TokenKind::kw_end)) {
      goto parseEntityDecl;
    }
  }

  consumeToken(TokenKind::kw_end);

  consumeIf(TokenKind::kw_entity);

  if (consumeIf(TokenKind::basic_identifier) || consumeIf(TokenKind::extended_identifier)) {
    return;
  }

  if (Tok.isNot(TokenKind::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();
};

void Parser::parseArchitectureDecl() {
  consumeToken(TokenKind::kw_architecture);

  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();

  if (Tok.isNot(TokenKind::kw_of)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::kw_of);

  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();

  if (Tok.isNot(TokenKind::kw_is)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_keyword);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::kw_is);

  bool parseDecl = true;
  while (parseDecl) {
    switch (Tok.getKind()) {
    case TokenKind::kw_signal:
      parseSignalDecl();
      break;
    case TokenKind::kw_subtype:
      parseSubtypeDecl();
      break;
    case TokenKind::kw_type:
      parseTypeDecl();
      break;
    default:
      parseDecl = false;
    }
  }

  if (Tok.isNot(TokenKind::kw_begin)) {
    std::cout << "expected begin keyword." << std::endl;
    return;
  }
  consumeToken(TokenKind::kw_begin);

  if (Tok.isNot(TokenKind::kw_end)) {
    std::cout << "expected begin keyword." << std::endl;
    return;
  }
  consumeToken(TokenKind::kw_end);

  consumeIf(TokenKind::kw_architecture);

  if (consumeIf(TokenKind::basic_identifier) || consumeIf(TokenKind::extended_identifier)) {
    return;
  }

  if (Tok.isNot(TokenKind::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  };
  consumeToken();
};

void Parser::parsePortClause() {
  consumeToken(TokenKind::kw_port);

  if (Tok.isNot(TokenKind::left_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_left_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken();
  parsePortInterfaceList();
  if (Tok.is(TokenKind::semicolon)) {
    // TODO: Superflicious semi
  }
  if (Tok.isNot(TokenKind::right_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_right_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken();
  if (Tok.isNot(TokenKind::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken();
};

void Parser::parseGenericClause() {
  consumeToken(TokenKind::kw_generic);

  if (Tok.isNot(TokenKind::left_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_left_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::left_parenthesis);

  parseGenericInterfaceList();
  if (Tok.is(TokenKind::semicolon)) {
    // TODO: Superflicious semi
  }

  if (Tok.isNot(TokenKind::right_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_right_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::right_parenthesis);

  if (Tok.isNot(TokenKind::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::semicolon);
};

void Parser::parseIdentifierList() {
  do {
    if (Tok.isAny(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
      consumeToken();
    }
  } while (consumeIf(TokenKind::comma));
}

void Parser::parseInterfaceSignalDeclaration() {
  consumeIf(TokenKind::kw_signal);
  parseIdentifierList();

  if (Tok.isNot(TokenKind::colon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_colon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::colon);

  if (Tok.isAny(TokenKind::kw_in, TokenKind::kw_out, TokenKind::kw_inout, TokenKind::kw_buffer,
                TokenKind::kw_linkage)) {
    consumeToken();
  }

  parseSubtypeIndication();

  if (consumeIf(TokenKind::kw_bus)) {
  }
  if (consumeIf(TokenKind::variable_assignment)) {
    parseExpr();
  }
}

void Parser::parseInterfaceConstantDeclaration() {
  consumeIf(TokenKind::kw_constant);
  parseIdentifierList();

  if (Tok.isNot(TokenKind::colon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_colon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(TokenKind::colon);

  parseSubtypeIndication();
  if (consumeIf(TokenKind::variable_assignment)) {
    parseExpr();
  }
}

void Parser::parseInterfaceVariableDeclaration() {
  consumeIf(TokenKind::kw_variable);
  parseIdentifierList();
  if (Tok.isAny(TokenKind::kw_in, TokenKind::kw_out, TokenKind::kw_inout, TokenKind::kw_buffer,
                TokenKind::kw_linkage)) {
    consumeToken();
  }
  parseSubtypeIndication();
  if (consumeIf(TokenKind::variable_assignment)) {
    parseExpr();
  }
}

void Parser::parseInterfaceFileDeclaration() {
  consumeIf(TokenKind::kw_file);
  parseIdentifierList();
  parseSubtypeIndication();
}

void Parser::parseInterfaceTypeDeclaration() {
  consumeToken(TokenKind::kw_type);

  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();
}

void Parser::parseInterfaceSubprogramDeclaration() {
  bool isFunction = false;
  bool hasParameters = false;

  if (!consumeIf(TokenKind::kw_procedure)) {
    if (consumeIf(TokenKind::kw_pure)) {

    } else if (consumeIf(TokenKind::kw_impure)) {
    }

    if (Tok.isNot(TokenKind::kw_function)) {
    }
    consumeToken(TokenKind::kw_function);
  }

  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
  }
  consumeToken();

  if (consumeIf(TokenKind::kw_parameter)) {
    hasParameters = true;
  }
  if (Tok.is(TokenKind::left_parenthesis)) {
    consumeToken(TokenKind::left_parenthesis);
    parseParameterInterfaceList();
    if (Tok.isNot(TokenKind::right_parenthesis)) {
      // TODO: Right paranthesis
    }
  } else if (hasParameters) {
    // TODO: No parameters with parameters
  }

  if (isFunction) {
    if (Tok.isNot(TokenKind::kw_return)) {
    }
    consumeToken(TokenKind::kw_return);
    parseName();
  }
}

void Parser::parseInterfacePackageDeclaration() {
  consumeToken(TokenKind::kw_package);

  if (Tok.isNot(TokenKind::basic_identifier, TokenKind::extended_identifier)) {
  }
  consumeToken();

  if (Tok.isNot(TokenKind::kw_is)) {
  }
  consumeToken(TokenKind::kw_is);

  if (Tok.isNot(TokenKind::kw_new)) {
  }
  consumeToken(TokenKind::kw_new);

  parseName();
  parseGenericMapAspect();
}

void Parser::parsePortInterfaceList() {
  bool isListAtEnd = false;
  do {
    switch (Tok.getKind()) {
    case TokenKind::basic_identifier:
    case TokenKind::extended_identifier:
    case TokenKind::kw_signal:
      parseInterfaceSignalDeclaration();
      break;
    case TokenKind::kw_constant:
    case TokenKind::kw_variable:
    case TokenKind::kw_file:
    case TokenKind::kw_impure:
    case TokenKind::kw_pure:
    case TokenKind::kw_function:
    case TokenKind::kw_procedure:
    case TokenKind::kw_package:
    case TokenKind::kw_type: {
      DiagnosticBuilder D = Diag->diagnose(diag::not_allowed_in_port_list);
      D.setLocation(Tok.getLocation());
    } break;
    default: {
      DiagnosticBuilder D = Diag->diagnose(diag::unexpected_token);
      D << Tok.getValue();
      D.setLocation(Tok.getLocation());
    }
    }

    switch (Tok.getKind()) {
    case TokenKind::basic_identifier:
    case TokenKind::extended_identifier:
    case TokenKind::kw_signal: {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    } break;
    case TokenKind::semicolon:
      consumeToken(TokenKind::semicolon);
      break;
    default:
      isListAtEnd = true;
    }

  } while (!isListAtEnd);
}

void Parser::parseGenericInterfaceList() {
  bool isListAtEnd = false;
  do {
    switch (Tok.getKind()) {
    case TokenKind::basic_identifier:
    case TokenKind::extended_identifier:
    case TokenKind::kw_constant:
      parseInterfaceConstantDeclaration();
      break;
    case TokenKind::kw_impure:
    case TokenKind::kw_pure:
    case TokenKind::kw_function:
    case TokenKind::kw_procedure:
      parseInterfaceSubprogramDeclaration();
      break;
    case TokenKind::kw_package:
      parseInterfacePackageDeclaration();
      break;
    case TokenKind::kw_type:
      parseInterfaceTypeDeclaration();
      break;
    case TokenKind::kw_signal:
    case TokenKind::kw_variable:
    case TokenKind::kw_file: {
      DiagnosticBuilder D = Diag->diagnose(diag::not_allowed_in_port_list);
      D.setLocation(Tok.getLocation());
    } break;
    default: {
      DiagnosticBuilder D = Diag->diagnose(diag::unexpected_token);
      D.setLocation(Tok.getLocation());
    }
    }

    switch (Tok.getKind()) {
    case TokenKind::basic_identifier:
    case TokenKind::extended_identifier:
    case TokenKind::kw_signal: {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    } break;
    case TokenKind::semicolon:
      break;
    default:
      isListAtEnd = true;
    }

  } while (!isListAtEnd);
}

void Parser::parseParameterInterfaceList() {
  bool isListAtEnd = false;
  do {
    switch (Tok.getKind()) {
    case TokenKind::basic_identifier:
    case TokenKind::extended_identifier:
    case TokenKind::kw_constant:
      parseInterfaceConstantDeclaration();
      break;
    case TokenKind::kw_impure:
    case TokenKind::kw_pure:
    case TokenKind::kw_function:
    case TokenKind::kw_procedure:
      parseInterfaceSubprogramDeclaration();
      break;
    case TokenKind::kw_package:
      parseInterfacePackageDeclaration();
      break;
    case TokenKind::kw_type:
      parseInterfaceTypeDeclaration();
      break;
    case TokenKind::kw_signal:
    case TokenKind::kw_variable:
    case TokenKind::kw_file: {
      DiagnosticBuilder D = Diag->diagnose(diag::not_allowed_in_port_list);
      D.setLocation(Tok.getLocation());
    } break;
    default: {
      DiagnosticBuilder D = Diag->diagnose(diag::unexpected_token);
      D.setLocation(Tok.getLocation());
    }
    }

    switch (Tok.getKind()) {
    case TokenKind::basic_identifier:
    case TokenKind::extended_identifier:
    case TokenKind::kw_signal: {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    } break;
    case TokenKind::semicolon:
      break;
    default:
      isListAtEnd = true;
    }

  } while (!isListAtEnd);
}
void Parser::parseAssociationList() {
  bool isListAtEnd = false;
  do {
    if (consumeIf(TokenKind::kw_open)) {
      goto ActualEnd;
    } else if (consumeIf(TokenKind::kw_inertial)) {
      goto ParseActual;
    }
    parseExpr();

    if (Tok.is(TokenKind::arrow)) {
      consumeToken();
    }
  ParseActual:
    parseExpr();

  ActualEnd:
    // TODO: recover
    if (!consumeIf(TokenKind::comma)) {
      isListAtEnd = true;
    }

  } while (!isListAtEnd);
}

void Parser::parseGenericMapAspect(bool isInterface) {
  if (Tok.isNot(TokenKind::kw_generic)) {
  }
  consumeToken(TokenKind::kw_generic);

  if (Tok.isNot(TokenKind::kw_map)) {
  }
  consumeToken(TokenKind::kw_map);

  if (Tok.isNot(TokenKind::left_parenthesis)) {
  }
  consumeToken(TokenKind::left_parenthesis);

  parseAssociationList();

  if (Tok.isNot(TokenKind::right_parenthesis)) {
  }
  consumeToken(TokenKind::right_parenthesis);
}