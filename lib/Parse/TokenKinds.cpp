#include "Parse/TokenKinds.h"
#include <string_view>
using namespace vc;

static const char *const TokNames[] = {
#define TOK(X) #X,
#define KEYWORD(X, Y) #X,
#include "Parse/TokenKinds.def"
    nullptr};

constexpr auto getTokenName(TokenKind Kind) -> std::string_view {
  if (Kind < TokenKind::NUM_TOKENS)
    return TokNames[static_cast<unsigned int>(Kind)];
  return std::string_view{};
}

constexpr auto getKeywordSpelling(TokenKind Kind) -> std::string_view {
  switch (Kind) {
#define KEYWORD(X, Y)                                                          \
  case TokenKind::kw_##X:                                                      \
    return #X;
#include "Parse/TokenKinds.def"
  default:
    break;
  }
  return std::string_view{};
}