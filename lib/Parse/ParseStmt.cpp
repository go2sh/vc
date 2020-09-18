#include "AST/Identifier.h"
#include "AST/Stmt.h"
#include "Parse/Parser.h"

using namespace vc;

Expr *Parser::parseSensitivityList() {
  do {
    Expr *E = parseName();
  } while (consumeIf(TokenKind::comma));
  return nullptr;
}

Stmt *Parser::parseProcessStmt(Identifier *Label, bool Postponed) {
  consumeToken(TokenKind::kw_process);

  if (consumeIf(TokenKind::left_parenthesis)) {
    if (consumeIf(TokenKind::kw_all)) {
      // Handle all
    } else {
      parseSensitivityList();
    }
    if (Tok.isNot(TokenKind::right_parenthesis)) {
    }
    consumeToken();
    consumeIf(TokenKind::kw_is);

    parseProcessDeclPart();

    if (Tok.isNot(TokenKind::kw_begin)) {
    }
    consumeToken();

    parseProcessStmtPart();

    if (Tok.isNot(TokenKind::kw_end)) {
    }
    consumeToken();

    if (consumeIf(TokenKind::kw_postponed) && !Postponed) {
    }

    if (Tok.isNot(TokenKind::kw_process)) {
    }
    consumeToken(TokenKind::kw_process);

    consumeIf(TokenKind::basic_identifier, TokenKind::extended_identifier);

    if (Tok.isNot(TokenKind::semicolon)) {
    }
    consumeToken(TokenKind::semicolon);
  }
  return nullptr;
}

void Parser::parseProcessDeclPart() {}

void Parser::parseProcessStmtPart() {}

// waveform ::= waveform_element { , waveform_element }
//            | unaffected
//
// waveform_element ::= value_expression [ after time_expression ]
//                   | null [ after time_expression ]
void Parser::parseWaveform() {
  Expr *R, *T;

  if (Tok.is(TokenKind::kw_unaffected)) {
    return;
  }

  do {
    if (consumeIf(TokenKind::kw_null)) {

    } else {
      R = parseExpr();
    }

    if (consumeIf(TokenKind::kw_after)) {
      T = parseExpr();
    }
  } while (consumeIf(TokenKind::comma));
}

// 9.3.3.1
// aggregate ::= ( element_association { , element_association } )
// element_association ::= [ choices => ] expression
// choices ::= choice { | choice }
// choice ::= simple_expression
//          | discrete_range
//          | element_simple_name
//          | others
void Parser::parseAggregate() {
  consumeToken(TokenKind::left_parenthesis);

  do {
    if (Tok.is(TokenKind::kw_others)) {
      consumeToken();
    } else if (Tok.is(TokenKind::left_parenthesis)) {
      parseSubtypeIndication();
    } else {
      parseSimpleExpr();
    }
  } while(false);
}

// 10.5.2
// simple_signal_assignment ::= simple_waveform_assignment
//                            | simple_force_assignment
//                            | simple_release_assignment
//
// simple_waveform_assignment ::= target <= [ delay_mechanism ] waveform ;
// simple_force_assignment ::= target <= force [ force_mode ] expression ;
// simple_release_assignment ::= target <= release [ force_mode ] ;
//
// force_mode ::= in | out
// delay_mechanism ::= transport
//                   | [ reject time_expression ] inertial
// target ::= name
//          | aggregate
void Parser::parseSimpleSignalAssignment() {
  if (Tok.is(TokenKind::left_parenthesis)) {
    parseAggregate();
  } else {
    parseName();
  }
}