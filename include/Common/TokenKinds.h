#ifndef VC_COMMON_TOKENKINDS
#define VC_COMMON_TOKENKINDS

enum TokenKind : unsigned short {
    #define TOK(X) X,
    #include "Common/TokenKinds.def"
    NUM_TOKENS
};


#endif