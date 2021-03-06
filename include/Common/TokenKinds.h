#ifndef VC_COMMON_TOKENKINDS
#define VC_COMMON_TOKENKINDS

namespace vc {
namespace tok {
enum TokenKind : unsigned short {
#define TOK(X) X,
#include "Common/TokenKinds.def"
  NUM_TOKENS
};

const char *getTokenName(TokenKind Kind);
const char *getKeywordSpelling(TokenKind Kind);

} // namespace tok
} // namespace vc

#endif