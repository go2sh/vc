#include "Token.h"

class Lexer {

  const char *bufferStart;
  const char *bufferEnd;

  const char *bufferPtr;

  bool isAtNewline;
  bool hasWhitespacePrefix;

public:

  Lexer(const char *inputBuffer);

  bool lex(Token &token);

private:
  bool lexStringLiteral(Token &token);
}