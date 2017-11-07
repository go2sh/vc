#ifndef TOKEN_H
#define TOKEN_H

#include "Common/TokenKinds.h"
#include <string>
#include <sys/types.h>

using namespace std;

namespace vc {
class Token {
  tok::TokenKind Kind;
  string Value;
  const char *Location;

public:
  Token() : Kind(tok::NUM_TOKENS) {};

  tok::TokenKind getKind() { return Kind; }
  void setKind(tok::TokenKind K) { Kind = K; }
  string &getValue() { return Value; };
  void setValue(std::string & S) { Value = S; };
  void setValue(const char *buf, size_t len) { Value = string(buf, len); }
  const char *getLocation() { return Location; };
  void setLocation(const char *Loc) { Location = Loc; };

  bool is(tok::TokenKind K) { return Kind == K; }
  
  bool isAny(tok::TokenKind K1) {
    return Kind == K1;
  };

  template <typename ...T>
  bool isAny(tok::TokenKind K1, T ...K) {
    return Kind == K1 || isAny(K...);
  };

  bool isNot(tok::TokenKind K) { return Kind != K; }
  bool isNot(tok::TokenKind K1,tok::TokenKind K2) { return Kind != K1 && Kind != K2;}
};

} // namespace vc
#endif