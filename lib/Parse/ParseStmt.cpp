#include "AST/Identifier.h"
#include "AST/Stmt.h"
#include "Parse/Parser.h"

using namespace vc;

Expr *Parser::parseSensitivityList() {
  do {
    Expr *E = parseName();
  } while (consumeIf(tok::comma));
  return nullptr;
}

Stmt *Parser::parseProcessStmt(Identifier *Label, bool Postponed) {
  consumeToken(tok::kw_process);

  if (consumeIf(tok::left_parenthesis)) {
    if (consumeIf(tok::kw_all)) {
      // Handle all
    } else {
      parseSensitivityList();
    }
    if (Tok.isNot(tok::right_parenthesis)) {
    }
    consumeToken();
    consumeIf(tok::kw_is);

    parseProcessDeclPart();

    if (Tok.isNot(tok::kw_begin)) {
    }
    consumeToken();

    parseProcessStmtPart();

    if (Tok.isNot(tok::kw_end)) {
    }
    consumeToken();

    if (consumeIf(tok::kw_postponed) && !Postponed) {
    }

    if (Tok.isNot(tok::kw_process)) {
    }
    consumeToken(tok::kw_process);

    consumeIf(tok::basic_identifier, tok::extended_identifier);

    if (Tok.isNot(tok::semicolon)) {
    }
    consumeToken(tok::semicolon);
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

  if (Tok.is(tok::kw_unaffected)) {
    return;
  }

  do {
    if (consumeIf(tok::kw_null)) {

    } else {
      R = parseExpr();
    }

    if (consumeIf(tok::kw_after)) {
      T = parseExpr();
    }
  } while (consumeIf(tok::comma));
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
  consumeToken(tok::left_parenthesis);

  do {
    if (Tok.is(tok::kw_others)) {
      consumeToken();
    } else if (Tok.is(tok::left_parenthesis)) {
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
  if (Tok.is(tok::left_parenthesis)) {
    parseAggregate();
  } else {
    parseName();
  }
}