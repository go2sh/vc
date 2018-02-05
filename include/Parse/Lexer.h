#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

#include "Common/SourceLocation.h"
#include "Common/MemoryBuffer.h"
#include "Diag/DiagnosticEngine.h"
#include "Parse/Token.h"

namespace vc {

uint32_t validateUTF8(const char *&Ptr, const char *End);

class Lexer {
  // Diagnostics
  DiagnosticEngine *Diag;

  // Buffer information
  SourceLocation FileLocation;
  const char *BufferStart;
  const char *BufferEnd;

  // Lexer configuration
  bool KeepComments;
  bool KeepWhitespaces;
  bool UseUTF8;

  // Lexer state
  const char *TokenStart;
  const char *CurrentPtr;
  bool isAtNewline;
  bool hasWhitespacePrefix;

public:
  Lexer(DiagnosticEngine &Diag, SourceLocation FileLocation, const MemoryBuffer *Buffer);

  void lex(Token &Result);

  void restoreToken(Token &T) { TokenStart = T.getValue().data(); };

private:
  void lexIdentifier(Token &Result, const char *CurrentPtr);
  void lexExtendedIdentifier(Token &Result);

  void lexStringLiteral(Token &Result, const char *CurrentPtr);
  void lexCharacterLiteral(Token &Result, const char *CurrentPtr);

  void lexNumber(Token &Result, const char *CurrentPtr);
  void lexInteger(Token &Resilt, const char *CurrentPtr);
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
    Result.setLocation(FileLocation.getLocWithOffset((uint32_t)(uintptr_t)(TokenEnd - BufferStart)));
    TokenStart = TokenEnd;
  }

  void formTokenWithValue(Token &Result, tok::TokenKind Kind,
                          const char *TokenEnd) {
    Result.setKind(Kind);
    Result.setLocation(FileLocation.getLocWithOffset((uint32_t)(uintptr_t)(TokenEnd - BufferStart)));
    Result.setValue(StringRef(BufferPtr, TokenEnd - BufferPtr));
    BufferPtr = TokenEnd;
  }
};

} // namespace vc
#endif
