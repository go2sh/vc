#ifndef LEXER_H
#define LEXER_H
#include <stdint.h>

#include <Common/TokenKinds.h>
#include <Lexer/Token.h>

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

  void Lex(Token &Result);

private:
  void lexIdentifier(Token &Result);
  void lexExtendedIdentifier(Token &Result);

  void lexStringLiteral(Token &Result);
  void lexCharacterLiteral(Token &Result);

  void lexNumber(Token &Result);
  void lexDecimalLiteral(Token &Result);
  void lexBasedLiteral(Token &Result);
  void lexBitStringLiteral(Token &Result);

  void LexCompoundDelimiter(Token &Result);

  void lexSingleLineComment(Token &Result);
  void lexMultiLineComment(Token &Result);

  void SkipSingleLineComment();
  void SkipMultilineComment();
  void SkipWhitespace(const char *CurrentPtr);

  void FormToken(Token &Result, tok::TokenKind Kind, const char *TokenEnd) {
    Result.setTokenKind(Kind);
    Result.setValue(BufferPtr, TokenEnd - BufferPtr);
    BufferPtr = TokenEnd;
  }
};

} // namespace vc
#endif
