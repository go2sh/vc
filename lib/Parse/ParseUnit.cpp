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
    while (Tok.isNot(tok::kw_end, tok::semicolon, tok::eof))
      consumeToken();

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
    switch (Tok.getKind()) {
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

void Parser::parsePortClause() {
  consumeToken(tok::kw_port);

  if (Tok.isNot(tok::left_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_left_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken();
  parsePortInterfaceList();
  if (Tok.is(tok::semicolon)) {
    // TODO: Superflicious semi
  }
  if (Tok.isNot(tok::right_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_right_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken();
  if (Tok.isNot(tok::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken();
};

void Parser::parseGenericClause() {
  consumeToken(tok::kw_generic);

  if (Tok.isNot(tok::left_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_left_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::left_parenthesis);

  parseGenericInterfaceList();
  if (Tok.is(tok::semicolon)) {
    // TODO: Superflicious semi
  }

  if (Tok.isNot(tok::right_parenthesis)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_right_parenthesis);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::right_parenthesis);

  if (Tok.isNot(tok::semicolon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::semicolon);
};

void Parser::parseIdentifierList() {
  do {
    if (Tok.isAny(tok::basic_identifier, tok::extended_identifier)) {
      consumeToken();
    }
  } while (consumeIf(tok::comma));
}

void Parser::parseInterfaceSignalDeclaration() {
  consumeIf(tok::kw_signal);
  parseIdentifierList();

  if (Tok.isNot(tok::colon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_colon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::colon);

  if (Tok.isAny(tok::kw_in, tok::kw_out, tok::kw_inout, tok::kw_buffer,
                tok::kw_linkage)) {
    consumeToken();
  }

  parseSubtypeIndication();

  if (consumeIf(tok::kw_bus)) {
  }
  if (consumeIf(tok::variable_assignment)) {
    parseExpr();
  }
}

void Parser::parseInterfaceConstantDeclaration() {
  consumeIf(tok::kw_constant);
  parseIdentifierList();

  if (Tok.isNot(tok::colon)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_colon);
    D.setLocation(Tok.getLocation());
    return;
  }
  consumeToken(tok::colon);

  parseSubtypeIndication();
  if (consumeIf(tok::variable_assignment)) {
    parseExpr();
  }
}

void Parser::parseInterfaceVariableDeclaration() {
  consumeIf(tok::kw_variable);
  parseIdentifierList();
  if (Tok.isAny(tok::kw_in, tok::kw_out, tok::kw_inout, tok::kw_buffer,
                tok::kw_linkage)) {
    consumeToken();
  }
  parseSubtypeIndication();
  if (consumeIf(tok::variable_assignment)) {
    parseExpr();
  }
}

void Parser::parseInterfaceFileDeclaration() {
  consumeIf(tok::kw_file);
  parseIdentifierList();
  parseSubtypeIndication();
}

void Parser::parseInterfaceTypeDeclaration() {
  consumeToken(tok::kw_type);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
    DiagnosticBuilder D = Diag->diagnose(diag::expected_identifier);
    D.setLocation(Tok.getLocation());
  }
  consumeToken();
}

void Parser::parseInterfaceSubprogramDeclaration() {
  bool isFunction = false;
  bool hasParameters = false;

  if (!consumeIf(tok::kw_procedure)) {
    if (consumeIf(tok::kw_pure)) {

    } else if (consumeIf(tok::kw_impure)) {
    }

    if (Tok.isNot(tok::kw_function)) {
    }
    consumeToken(tok::kw_function);
  }

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
  }
  consumeToken();

  if (consumeIf(tok::kw_parameter)) {
    hasParameters = true;
  }
  if (Tok.is(tok::left_parenthesis)) {
    consumeToken(tok::left_parenthesis);
    parseParameterInterfaceList();
    if (Tok.isNot(tok::right_parenthesis)) {
      // TODO: Right paranthesis
    }
  } else if (hasParameters) {
    // TODO: No parameters with parameters
  }

  if (isFunction) {
    if (Tok.isNot(tok::kw_return)) {
    }
    consumeToken(tok::kw_return);
    parseName();
  }
}

void Parser::parseInterfacePackageDeclaration() {
  consumeToken(tok::kw_package);

  if (Tok.isNot(tok::basic_identifier, tok::extended_identifier)) {
  }
  consumeToken();

  if (Tok.isNot(tok::kw_is)) {
  }
  consumeToken(tok::kw_is);

  if (Tok.isNot(tok::kw_new)) {
  }
  consumeToken(tok::kw_new);

  parseName();
  parseGenericMapAspect();
}

void Parser::parsePortInterfaceList() {
  bool isListAtEnd = false;
  do {
    switch (Tok.getKind()) {
    case tok::basic_identifier:
    case tok::extended_identifier:
    case tok::kw_signal:
      parseInterfaceSignalDeclaration();
      break;
    case tok::kw_constant:
    case tok::kw_variable:
    case tok::kw_file:
    case tok::kw_impure:
    case tok::kw_pure:
    case tok::kw_function:
    case tok::kw_procedure:
    case tok::kw_package:
    case tok::kw_type: {
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
    case tok::basic_identifier:
    case tok::extended_identifier:
    case tok::kw_signal: {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    } break;
    case tok::semicolon:
      consumeToken(tok::semicolon);
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
    case tok::basic_identifier:
    case tok::extended_identifier:
    case tok::kw_constant:
      parseInterfaceConstantDeclaration();
      break;
    case tok::kw_impure:
    case tok::kw_pure:
    case tok::kw_function:
    case tok::kw_procedure:
      parseInterfaceSubprogramDeclaration();
      break;
    case tok::kw_package:
      parseInterfacePackageDeclaration();
      break;
    case tok::kw_type:
      parseInterfaceTypeDeclaration();
      break;
    case tok::kw_signal:
    case tok::kw_variable:
    case tok::kw_file: {
      DiagnosticBuilder D = Diag->diagnose(diag::not_allowed_in_port_list);
      D.setLocation(Tok.getLocation());
    } break;
    default: {
      DiagnosticBuilder D = Diag->diagnose(diag::unexpected_token);
      D.setLocation(Tok.getLocation());
    }
    }

    switch (Tok.getKind()) {
    case tok::basic_identifier:
    case tok::extended_identifier:
    case tok::kw_signal: {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    } break;
    case tok::semicolon:
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
    case tok::basic_identifier:
    case tok::extended_identifier:
    case tok::kw_constant:
      parseInterfaceConstantDeclaration();
      break;
    case tok::kw_impure:
    case tok::kw_pure:
    case tok::kw_function:
    case tok::kw_procedure:
      parseInterfaceSubprogramDeclaration();
      break;
    case tok::kw_package:
      parseInterfacePackageDeclaration();
      break;
    case tok::kw_type:
      parseInterfaceTypeDeclaration();
      break;
    case tok::kw_signal:
    case tok::kw_variable:
    case tok::kw_file: {
      DiagnosticBuilder D = Diag->diagnose(diag::not_allowed_in_port_list);
      D.setLocation(Tok.getLocation());
    } break;
    default: {
      DiagnosticBuilder D = Diag->diagnose(diag::unexpected_token);
      D.setLocation(Tok.getLocation());
    }
    }

    switch (Tok.getKind()) {
    case tok::basic_identifier:
    case tok::extended_identifier:
    case tok::kw_signal: {
      DiagnosticBuilder D = Diag->diagnose(diag::expected_semicolon);
      D.setLocation(Tok.getLocation());
    } break;
    case tok::semicolon:
      break;
    default:
      isListAtEnd = true;
    }

  } while (!isListAtEnd);
}
void Parser::parseAssociationList() {
  bool isListAtEnd = false;
  do {
    if (consumeIf(tok::kw_open)) {
      goto ActualEnd;
    } else if (consumeIf(tok::kw_inertial)) {
      goto ParseActual;
    }
    parseExpr();

    if (Tok.is(tok::arrow)) {
      consumeToken();
    }
  ParseActual:
    parseExpr();

  ActualEnd:
    // TODO: recover
    if (!consumeIf(tok::comma)) {
      isListAtEnd = true;
    }

  } while (!isListAtEnd);
}

void Parser::parseGenericMapAspect(bool isInterface) {
  if (Tok.isNot(tok::kw_generic)) {
  }
  consumeToken(tok::kw_generic);

  if (Tok.isNot(tok::kw_map)) {
  }
  consumeToken(tok::kw_map);

  if (Tok.isNot(tok::left_parenthesis)) {
  }
  consumeToken(tok::left_parenthesis);

  parseAssociationList();

  if (Tok.isNot(tok::right_parenthesis)) {
  }
  consumeToken(tok::right_parenthesis);
}