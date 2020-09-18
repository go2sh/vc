#ifndef VC_PARSER_H
#define VC_PARSER_H

#include <cassert>

#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Identifier.h"
#include "AST/Stmt.h"
#include "AST/Type.h"
#include "Diag/DiagnosticEngine.h"
#include "Parse/Token.h"
#include "Parse/Lexer.h"

namespace vc {
class Parser {
  DiagnosticEngine *Diag;
  Lexer *L;
  Token Tok;

public:
  Parser(DiagnosticEngine &Diag, Lexer *L) : Diag(&Diag), L(L){};

public:
  void consumeToken() {
    assert(Tok.isNot(TokenKind::eof) && "Lexing past eof!");
    L->lex(Tok);
  }

  void consumeToken(TokenKind K) {
    assert(Tok.is(K) && "Consuming wrong token kind");
    return consumeToken();
  }

  bool consumeIf(TokenKind K) {
    if (Tok.isNot(K))
      return false;
    consumeToken(K);
    return true;
  }

  template <typename... T> bool consumeIf(TokenKind K1, T... K) {
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

  void parseLibraryUnit();

  void parseEntityDecl();
  void parsePackageDecl();
  void parseContextDecl();
  void parseArchitectureDecl();
  void parsePackageBody();

  void parsePortClause();
  void parseGenericClause();

  void parseIdentifierList();
  void parseInterfaceSignalDeclaration();
  void parseInterfaceVariableDeclaration();
  void parseInterfaceConstantDeclaration();
  void parseInterfaceFileDeclaration();
  void parseInterfaceTypeDeclaration();
  void parseInterfaceSubprogramDeclaration();
  void parseInterfacePackageDeclaration();
  void parsePortInterfaceList();
  void parseGenericInterfaceList();
  void parseParameterInterfaceList();

  void parseAssociationList();
  
  void parseGenericMapAspect(bool isInterface = false);

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

  void parseWaveform();
  void parseAggregate();
  void parseSimpleSignalAssignment();


private:
};
}; // namespace vc
#endif