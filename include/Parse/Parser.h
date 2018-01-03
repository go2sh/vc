#ifndef VC_PARSER_H
#define VC_PARSER_H

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Identifier.h"
#include "AST/Stmt.h"
#include "AST/Type.h"
#include "Common/TokenKinds.h"
#include "Diag/DiagnosticEngine.h"
#include "Parse/Lexer.h"
#include "Parse/Token.h"
#include <cassert>

namespace vc {
class Parser {
  DiagnosticEngine *Diag;
  Lexer *L;
  Token Tok;

public:
  Parser(DiagnosticEngine &Diag, Lexer *L) : Diag(&Diag), L(L){};

public:
  void consumeToken() {
    assert(Tok.isNot(tok::eof) && "Lexing past eof!");
    L->lex(Tok);
  }

  void consumeToken(tok::TokenKind K) {
    assert(Tok.is(K) && "Consuming wrong token kind");
    return consumeToken();
  }

  bool consumeIf(tok::TokenKind K) {
    if (Tok.isNot(K))
      return false;
    consumeToken(K);
    return true;
  }

  template <typename... T> bool consumeIf(tok::TokenKind K1, T... K) {
    if (!Tok.isAny(K1, K...))
      return false;
    consumeToken();
    return true;
  };

public:
  bool parseDesignFile();
  bool parseDesignUnit();
  void parseContextClause();

  void parseLibraryClause();
  void parseUseClause();
  bool parseContextReference();

  void parsePrimaryUnit();
  void parseSecondaryUnit();

  void parseEntityDecl();
  void parsePackageDecl();
  void parseContextDecl();
  void parseArchitectureDecl();
  void parsePackageBody();

  void parsePortClause();
  void parseGenericClause();

  Decl *parseSignalDecl(bool Interface = false);
  Decl *parseSubtypeDecl();
  Decl *parseTypeDecl();

  void parseArrayTypeDef();
  void parseRecordTypeDef();
  void parseEnumTypeDef();
  void parseRangeTypeDef();

  Type *parseSubtypeIndication();
  void parseArrayOrRecordConstr();
  void parseRecordConstr();
  void parseRangeConstr(bool AllowUnconstrained = false);

  Expr *parseName();
  Expr *parseSimpleName();
  Expr *parseSelectedName();

  Expr *parseExpr();
  Expr *parseLogicalExpr();
  Expr *parseRelationExpr();
  Expr *parseShiftExpr();
  Expr *parseSimpleExpr();
  Expr *parseTermExpr();
  Expr *parseFactorExpr();
  Expr *parsePrimaryExpr();

  Expr *parseSensitivityList();
  Stmt *parseProcessStmt(Identifier *I, bool Postponed);
  void parseProcessDeclPart();
  void parseProcessStmtPart();

private:
};
}; // namespace vc
#endif