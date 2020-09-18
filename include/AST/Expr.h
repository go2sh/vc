#ifndef VC_EXPR_H
#define VC_EXPR_H

#include <string_view>

#include "AST/Identifier.h"

namespace vc {

enum class ExprKind {
  AnyExpr,
  ErrorExpr,
  DeclRefExpr,
  SelectedExpr,
  AttrExpr,
  CharLiteralExpr,
  StringLiteralExpr
};

class Expr {
  ExprKind Kind;
  bool IsSimple;

public:
  Expr(ExprKind Kind) : Kind(Kind) { IsSimple = true; }

  ExprKind getKind() { return Kind; }
  bool isSimple() { return IsSimple; }
};

class AnyExpr : public Expr {
public:
  AnyExpr() : Expr(ExprKind::AnyExpr) {}
};

class ErrorExpr : public Expr {
public:
  ErrorExpr() : Expr(ExprKind::ErrorExpr) {}
};

class DeclRefExpr : public Expr {
  Identifier Name;

public:
  DeclRefExpr(Identifier N) : Expr(ExprKind::DeclRefExpr), Name(N) {}
  void setName(Identifier N) { Name = N; }
};

class SelectedExpr : public Expr {
  Expr *Prefix;
  Expr *Suffix;

public:
  SelectedExpr(Expr *Prefix, Expr *Suffix = nullptr)
      : Expr(ExprKind::SelectedExpr), Prefix(Prefix), Suffix(Suffix) {}
  void setPrefix(Expr *E) { Prefix = E; }
  void setSuffix(Expr *E) { Suffix = E; }
};

class LiteralExpr : public Expr {
  std::string_view Value;

public:
  LiteralExpr(ExprKind Kind, std::string_view Val) : Expr(Kind), Value(Val) {}
  void setName(std::string &N) { Value = N; }
};

class StringLiteralExpr : public LiteralExpr {

public:
  StringLiteralExpr(std::string_view Val)
      : LiteralExpr(ExprKind::StringLiteralExpr, Val) {}
};

class CharLiteralExpr : public LiteralExpr {
public:
  CharLiteralExpr(std::string_view Val)
      : LiteralExpr(ExprKind::CharLiteralExpr, Val) {}
};
}; // namespace vc

#endif
