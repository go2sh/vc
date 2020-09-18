#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <string_view>
#include <sys/types.h>

#include "Common/SourceLocation.h"
#include "Parse/TokenKinds.h"

namespace vc {
class Token {
  TokenKind Kind{TokenKind::NUM_TOKENS};
  std::string_view Value;
  SourceLocation Location;

public:
  Token() = default;
  Token(TokenKind Kind, SourceLocation Location, std::string_view Value)
      : Kind(Kind), Location(Location), Value(Value) {}

  auto getKind() -> TokenKind { return Kind; }
  //void setKind(TokenKind K) { Kind = K; }
  auto getValue() -> std::string_view { return Value; }
  //void setValue(std::string_view S) { Value = S; };
  auto getLocation() -> SourceLocation { return Location; }
  //void setLocation(SourceLocation Loc) { Location = Loc; };

  auto is(TokenKind K) -> bool { return Kind == K; }

  auto isAny(TokenKind K) -> bool { return Kind == K; }
  template <typename... T> auto isAny(T... K) -> bool {
    return (isAny(K) || ...);
  };

  auto isNot(TokenKind K) -> bool { return Kind != K; }
  template <typename... T> auto isNot(T... K) -> bool {
    return (isNot(K) && ...);
  };
};

} // namespace vc
#endif