#ifndef TOKEN_H
#define TOKEN_H

#include "Common/SourceLocation.h"
#include "Common/StringRef.h"
#include "Common/TokenKinds.h"
#include <string>
#include <sys/types.h>

using namespace std;

namespace vc {
class Token {
  tok::TokenKind Kind;
  StringRef Value;
  SourceLocation Location;

public:
  Token() : Kind(tok::NUM_TOKENS){};
  Token(tok::TokenKind Kind, SourceLocation Location, StringRef Value)
      : Kind(Kind), Location(Location), Value(Value) {}

  tok::TokenKind getKind() { return Kind; }
  void setKind(tok::TokenKind K) { Kind = K; }
  StringRef getValue() { return Value; };
  void setValue(StringRef S) { Value = S; };
  SourceLocation getLocation() { return Location; };
  void setLocation(SourceLocation Loc) { Location = Loc; };

  bool is(tok::TokenKind K) { return Kind == K; }

  bool isAny(tok::TokenKind K1) { return Kind == K1; };

  template <typename... T> bool isAny(tok::TokenKind K1, T... K) {
    return Kind == K1 || isAny(K...);
  };

  bool isNot(tok::TokenKind K) { return Kind != K; }

  template <typename... T> bool isNot(tok::TokenKind K1, T... K) {
    return Kind != K1 && isNot(K...);
  };
};

} // namespace vc
#endif