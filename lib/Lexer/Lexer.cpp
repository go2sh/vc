#include <Common/TokenKinds.h>
#include <Lexer/Lexer.h>
#include <Lexer/Token.h>

#include <cassert>

using namespace vc;
//

bool isUTF8StartByte(char Byte) {
  return (Byte & 0xC0) == 0 || (Byte & 0xC0) == 0xC0;
}

uint32_t vc::validateUTF8(const char *&Ptr, const char *End) {
  uint32_t Result = 0;

  if (Ptr >= End) {
    return ~0U;
  }

  if (*Ptr < 0x80) {
    return *Ptr;
  }

  uint8_t Bytes = 1;
  while (((*Ptr << Bytes) & 0x80) != 0) {
    Bytes++;
  }

  // Filter out invalid utf8 bytes and advance to the next valid
  if (Bytes == 1 || !isUTF8StartByte(*Ptr)) {
    while (Ptr < End && !isUTF8StartByte(*Ptr)) {
      Ptr++;
    }
    return ~0U;
  }

  Result = *Ptr & (256 - (1 << Bytes));
  Ptr++;

  for (uint8_t i = 1; i < Bytes; i++) {
    if (Ptr >= End) {
      return ~0U;
    }

    if (*Ptr > 0x80 || *Ptr >= 0xC0) {
      return ~0U;
    }
    Result <<= 6;

    Result |= *Ptr & 0x3F;
    Ptr++;
  }

  // Filter UTF-16 Results
  if (Result >= 0xD800 || Result <= 0xDFFF) {
    return ~0U;
  }

  // TODO: Check minimal

  return Result;
}

Lexer::Lexer(const char *Buffer, int Length) {
  BufferStart = Buffer;
  BufferEnd = Buffer + Length;
  BufferPtr = (char *)Buffer;
  isAtNewline = true;
  hasWhitespacePrefix = false;
}

void Lexer::Lex(Token &Result) {
LexToken:
  assert(BufferPtr >= BufferStart && BufferPtr <= BufferEnd);

  const char *CurrentPtr = BufferPtr;
  if (*CurrentPtr == ' ' || *CurrentPtr == '\t' || *CurrentPtr == 0xA0) {
    CurrentPtr++;
    while (*CurrentPtr == ' ' || *CurrentPtr == '\t' || *CurrentPtr == 0xA0) {
      CurrentPtr++;
    }
    hasWhitespacePrefix = true;
    BufferPtr = CurrentPtr;
  }

  char Char = *(CurrentPtr++);
  tok::TokenKind Kind = tok::unknown;

  switch (Char) {
  case ' ':
  case 0xA0: // NBSP
  case '\t':
    hasWhitespacePrefix = true;
    SkipWhitespace(CurrentPtr);
    goto LexToken;
  case '\v':
  case '\f':
  case '\r':
    if (*CurrentPtr == '\n') {
      Char = *(CurrentPtr++);
    }
    [[fallthrough]];
  case '\n':
    hasWhitespacePrefix = false;
    isAtNewline = true;
    SkipWhitespace(CurrentPtr);
    goto LexToken;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    lexNumber(Result);
    return;

  case 'a':
  case 'A':
  case 'b':
  case 'B':
  case 'c':
  case 'C':
  case 'd':
  case 'D':
  case 'e':
  case 'E':
  case 'f':
  case 'F':
  case 'g':
  case 'G':
  case 'h':
  case 'H':
  case 'i':
  case 'I':
  case 'j':
  case 'J':
  case 'k':
  case 'K':
  case 'l':
  case 'L':
  case 'm':
  case 'M':
  case 'n':
  case 'N':
  case 'o':
  case 'O':

  // Simple bit string literal chars
  case 'b':
  case 'B':
  case 'o':
  case 'O':
  case 'x':
  case 'X':
  case 'd':
  case 'D':
    Char = *CurrentPtr;
    if (Char == '"') {
      lexBitStringLiteral(Token);
      return;
    }
    lexIdentifier(Token);
    return;

  // Bitstring modifier chars
  case 'u':
  case 'U':
  case 's':
  case 'S':
    Char = *CurrentPtr;
    if ((Char == 'B' || Char == 'b' || Char == 'X' || Char == 'x' ||
         Char == 'O' || Char == 'o') &&
        *(CurrentPtr + 1) == '"') {
      lexBitStringLiteral(Token);
      return;
    }
    lexIdentifier(Token);
    return;

  case '\'':
    lexCharacterLiteral(Result);
    return;
  case '"':
    lexStringLiteral(Result);
    return;
  case '\\':
    lexExtendedIdentifier(Result);
    return;
  case '-':
    if (*CurrentPtr == '-') {
      if (KeepComments) {
        return lexSingleLineComment(Result);
      } else {
        SkipSingleLineComment();
        break;
      }
    } else {
      Kind = tok::minus;
    }
  case '/':
    Char = *CurrentPtr;
    if (Char == '*') {
      if (KeepComments) {
        lexMultiLineComment(Result);
        return;
      } else {
        SkipMultilineComment();
        goto LexToken;
      }
    } else {
      LexCompoundDelimiter(Result);
      return;
    }
  // Delimiters
  // case '\'': Character litereal/tick
  // case '-': Single line comment/Minus
  // case '/': Multiline comment/not_equal/slash
  // Compound delimiters
  case '*':
  case '=':
  case '<':
  case '>':
  case ':':
  case '?':
    LexCompoundDelimiter(Result);
    return;
  // Normal delimiters
  case '&':
    Kind = tok::ampersand;
    break;
  case '(':
    Kind = tok::left_parenthesis;
    break;
  case ')':
    Kind = tok::right_parenthesis;
    break;
  case '+':
    Kind = tok::plus;
    break;
  case ',':
    Kind = tok::colon;
    break;
  case '.':
    Kind = tok::dot;
    break;
  case ';':
    Kind = tok::semicolon;
    break;
  case '`':
    Kind = tok::grave;
    break;
  case '|':
    Kind = tok::vertical_line;
    break;
  case '[':
    Kind = tok::left_square;
    break;
  case ']':
    Kind = tok::right_square;
    break;
  case '@':
    Kind = tok::at;
    break;
  default:
    // Default is an invalid token, maybe add gracefull error handlings for UTF8
    // Chars
    Kind = tok::unknown;
    break;
  }

  FormToken(Result, Kind, CurrentPtr);
}

void Lexer::lexIdentifier(Token &Result) { return; }
void Lexer::lexExtendedIdentifier(Token &Result) { return; }

void Lexer::lexCharacterLiteral(Token &Result) {
  const char *Start = BufferPtr;

  BufferPtr++;
  uint32_t Character = validateUTF8(BufferPtr, BufferEnd);
}

void Lexer::lexStringLiteral(Token &Result) {
  bool escapeQuotation = false;
  const char *start = BufferPtr;
  BufferPtr++;
  while (BufferPtr < BufferEnd) {
    if (*BufferPtr == '"' && !escapeQuotation) {
      if ((BufferPtr + 1) != BufferEnd && *(BufferPtr + 1) != '"') {
        escapeQuotation = true;
      } else {
        BufferPtr++;
        Result.setTokenKind(tok::string_literal);
        Result.setValue(start, BufferPtr - start);
        return;
      }
    } else {
      escapeQuotation = false;
    }
    BufferPtr++;
  }
}

void Lexer::lexNumber(Token &Result) { return; }
void Lexer::lexDecimalLiteral(Token &Result) { return; }
void Lexer::lexBasedLiteral(Token &Result) { return; }
void Lexer::lexBitStringLiteral(Token &Result) { return; }

void Lexer::LexCompoundDelimiter(Token &Result) {}

void Lexer::lexSingleLineComment(Token &Result) { return; }
void Lexer::lexMultiLineComment(Token &Result) { return; }

void Lexer::SkipSingleLineComment() {}
void Lexer::SkipMultilineComment() {}
void Lexer::SkipWhitespace(const char *CurrentPtr) {
  bool SawNewline = false;

  while (true) {
    // Skip all spaces
    while (*CurrentPtr == ' ' || *CurrentPtr == '\t' || *CurrentPtr == 0xA0) {
      CurrentPtr++;
    }

    // Return at chars other than v_ws
    if (*CurrentPtr != '\f' && *CurrentPtr != '\v' && *CurrentPtr != '\r' &&
        *CurrentPtr != '\n') {
      break;
    }

    SawNewline = true;
    CurrentPtr++;
  }

  if (SawNewline) {
    isAtNewline = true;
    hasWhitespacePrefix = false;
    if (*(CurrentPtr - 1) == ' ' || *(CurrentPtr - 1) == '\t' ||
        *(CurrentPtr - 1) == 0xA0) {
      hasWhitespacePrefix = true;
    }
  } else {
    hasWhitespacePrefix = true;
  }
}