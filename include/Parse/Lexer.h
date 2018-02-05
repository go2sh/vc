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
  void lexIdentifier(Token &Result);
  void lexExtendedIdentifier(Token &Result);

  void lexStringLiteral(Token &Result);
  void lexCharacterLiteral(Token &Result);

  void lexNumber(Token &Result);
  void lexInteger(Token &Resilt);
  void lexDecimalLiteral(Token &Result);
  void lexBasedLiteral(Token &Result);
  void lexBitStringLiteral(Token &Result);

  void lexCompoundDelimiter(Token &Result);

  void lexSingleLineComment(Token &Result);
  void lexMultiLineComment(Token &Result);

  void skipSingleLineComment();
  void skipMultilineComment();
  void skipWhitespace();

  void formToken(Token &Result, tok::TokenKind Kind) {
    Result.setKind(Kind);
    Result.setLocation(FileLocation.getLocWithOffset((uint32_t)(uintptr_t)(TokenStart - BufferStart)));
    TokenStart = CurrentPtr;
  }

  void formTokenWithValue(Token &Result, tok::TokenKind Kind) {
    Result.setKind(Kind);
    Result.setLocation(FileLocation.getLocWithOffset((uint32_t)(uintptr_t)(TokenStart - BufferStart)));
    Result.setValue(StringRef(TokenStart, (size_t)(CurrentPtr - TokenStart)));
    TokenStart = CurrentPtr;
  }
};

} // namespace vc
#endif
