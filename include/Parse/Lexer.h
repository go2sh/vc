#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

#include "Common/TokenKinds.h"
#include "Parse/Token.h"

namespace vc {

uint32_t validateUTF8(const char *&Ptr, const char *End);

class Lexer {

  const char *BufferStart;
  const char *BufferEnd;

  const char *BufferPtr;

  bool isAtNewline;
  bool hasWhitespacePrefix;
  bool KeepComments;
  bool KeepWhitespaces;
  bool UseUTF8;

public:
  Lexer(const char *inputBuffer, int length);

  void lex(Token &Result);

  void restoreToken(Token &T) {
    BufferPtr = T.getValue().data();
  };

private:
  void lexIdentifier(Token &Result, const char *CurrentPtr);
  void lexExtendedIdentifier(Token &Result);

  void lexStringLiteral(Token &Result, const char * CurrentPtr);
  void lexCharacterLiteral(Token &Result, const char *CurrentPtr);

  void lexNumber(Token &Result, const char *CurrentPtr);
  void lexDecimalLiteral(Token &Result, const char *CurrentPtr);
  void lexBasedLiteral(Token &Result, const char *CurrentPtr);
  void lexBitStringLiteral(Token &Result, const char *CurrentPtr);

  void lexCompoundDelimiter(Token &Result, const char *CurrentPtr);

  void lexSingleLineComment(Token &Result);
  void lexMultiLineComment(Token &Result);

  void skipSingleLineComment();
  void skipMultilineComment();
  void skipWhitespace(const char *CurrentPtr);

  void formToken(Token &Result, tok::TokenKind Kind, const char *TokenEnd) {
    Result.setKind(Kind);
    Result.setLocation(BufferPtr);
    BufferPtr = TokenEnd;
  }

  void formTokenWithValue(Token &Result, tok::TokenKind Kind, const char *TokenEnd) {
    Result.setKind(Kind);
    Result.setLocation(BufferPtr);
    Result.setValue(BufferPtr, TokenEnd - BufferPtr);
    BufferPtr = TokenEnd;
  }
};

} // namespace vc
#endif
