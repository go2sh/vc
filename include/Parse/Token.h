#ifndef TOKEN_H
#define TOKEN_H

#include <string>

#include <sys/types.h>

using namespace std;

#include <Common/TokenKinds.h>

namespace vc {
class Token {
  tok::TokenKind kind;
  string value;

public:
  tok::TokenKind getTokenKind() { return kind; }
  void setTokenKind(tok::TokenKind k) { kind = k; }
  string getValue() { return value; }
  void setValue(const char *buf, size_t len) { value = string(buf, len); }
};

} // namespace vc
#endif