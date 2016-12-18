#ifndef VC_COMMON_TOKENKINDS
#define VC_COMMON_TOKENKINDS

enum TokenKind : unsigned short {
    #define TOK(X) X,
    #include "common/TokenKinds.def"
    NUM_TOKENS
}

enum TokenKind : unsigned short {
    #define KEYWORD(X) X,
    #include "common/TokenKinds.def"
    NUM_KEYWORDS
}

#endif