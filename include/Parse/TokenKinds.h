#ifndef VC_COMMON_TOKENKINDS
#define VC_COMMON_TOKENKINDS
#include <string_view>

namespace vc {
enum class TokenKind : unsigned short {
#define TOK(X) X,
#include "TokenKinds.def"
  NUM_TOKENS
};

constexpr auto getTokenName(TokenKind Kind) -> std::string_view;
constexpr auto getKeywordSpelling(TokenKind Kind) -> std::string_view;

} // namespace vc

#endif