#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

#include "Common/MemoryBuffer.h"
#include "Common/SourceLocation.h"
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

  // Lexer state
  const char *TokenStart;
  const char *CurrentPtr;
  bool isAtNewline;
  bool hasWhitespacePrefix;
  Token NextToken;

public:
  Lexer(DiagnosticEngine &Diag, SourceLocation FileLocation,
        const MemoryBuffer *Buffer);

  void lex(Token &Result);
  void restoreToken(Token &T) { TokenStart = T.getValue().data(); };

private:
  void lexIdentifier();
  void lexExtendedIdentifier();

  void lexStringLiteral();
  void lexCharacterLiteral();

  void lexNumber();
  void lexInteger();
  void lexDecimalLiteral();
  void lexBasedLiteral();
  void lexBitStringLiteral();

  void lexCompoundDelimiter();

  bool skipSingleLineComment();
  bool skipMultilineComment();
  bool skipWhitespace();

  void formToken(tok::TokenKind Kind) {
    NextToken.setKind(Kind);
    NextToken.setValue(StringRef(TokenStart, (size_t)(CurrentPtr - TokenStart)));
    NextToken.setLocation(FileLocation.getLocWithOffset(
        (uint32_t)(uintptr_t)(TokenStart - BufferStart)));
    TokenStart = CurrentPtr;
  }
};

} // namespace vc
#endif
