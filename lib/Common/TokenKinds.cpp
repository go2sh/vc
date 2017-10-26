#include "Common/TokenKinds.h"
using namespace vc;

static const char * const TokNames[] = {
#define TOK(X) #X,
#define KEYWORD(X,Y) #X,
#include "Common/TokenKinds.def"
  nullptr
};

const char *tok::getTokenName(TokenKind Kind) {
  if (Kind < tok::NUM_TOKENS)
    return TokNames[Kind];
  return nullptr;
}

const char *tok::getKeywordSpelling(TokenKind Kind) {
  switch (Kind) {
#define KEYWORD(X,Y) case kw_ ## X: return #X;
#include "Common/TokenKinds.def"
    default: break;
  }
  return nullptr;
}