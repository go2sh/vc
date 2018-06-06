#include <cassert>
#include <iostream>

#include "Common/CharInfo.h"
#include "Common/TokenKinds.h"
#include "Parse/DiagnosticsLex.h"
#include "Parse/Lexer.h"
#include "Parse/Token.h"

using namespace vc;

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

Lexer::Lexer(DiagnosticEngine &Diag, SourceLocation FileLocation,
             const MemoryBuffer *Buffer)
    : Diag(&Diag), FileLocation(FileLocation) {
  BufferStart = Buffer->getStart();
  BufferEnd = Buffer->getEnd();
  TokenStart = BufferStart;
  CurrentPtr = BufferStart;
  isAtNewline = true;
  hasWhitespacePrefix = false;
}

void Lexer::lex(Token &Result) {
LexToken:
  assert(CurrentPtr >= BufferStart && CurrentPtr <= BufferEnd);

  if (*CurrentPtr == ' ' || *CurrentPtr == '\t' || *CurrentPtr == 0xA0) {
    CurrentPtr++;
    while (*CurrentPtr == ' ' || *CurrentPtr == '\t' || *CurrentPtr == 0xA0) {
      CurrentPtr++;
    }
    hasWhitespacePrefix = true;
    TokenStart = CurrentPtr;
  }

  char Char = *(CurrentPtr++);
  tok::TokenKind Kind = tok::unknown;

  switch (Char) {
  case 0:
    if (CurrentPtr - 1 != BufferEnd) {
      //
      goto LexToken;
    }
    CurrentPtr--;
    formToken(Result, tok::eof);
    return;
  case ' ':
  case (char)0xA0: // NBSP
  case '\t':
    hasWhitespacePrefix = true;
    skipWhitespace();
    goto LexToken;
  case '\v':
  case '\f':
  case '\r':
    if (*CurrentPtr == '\n') {
      Char = *(CurrentPtr++);
    }
  //[[fallthrough]];
  case '\n':
    hasWhitespacePrefix = false;
    isAtNewline = true;
    skipWhitespace();
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
  // case 'b':
  // case 'B':
  case 'c':
  case 'C':
  // case 'd':
  // case 'D':
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
  // case 'o':
  // case 'O':
  case 'p':
  case 'P':
  case 'q':
  case 'Q':
  case 'r':
  case 'R':
  // case 's':
  // case 'S':
  case 't':
  case 'T':
  // case 'u':
  // case 'U':
  case 'v':
  case 'V':
  case 'w':
  case 'W':
  // case 'x':
  // case 'X':
  case 'y':
  case 'Y':
  case 'z':
  case 'Z':
    lexIdentifier(Result);
    return;

  // Simple bit string literal chars
  case 'b':
  case 'B':
  case 'o':
  case 'O':
  case 'x':
  case 'X':
  case 'd':
  case 'D':
    if (*CurrentPtr == '"') {
      CurrentPtr++;
      lexBitStringLiteral(Result);
      return;
    }
    lexIdentifier(Result);
    return;

  // Bitstring modifier chars
  case 'u':
  case 'U':
  case 's':
  case 'S':
    Char = *CurrentPtr;
    if (IsBitStringBase(*CurrentPtr) && *(CurrentPtr + 1) == '"') {
      CurrentPtr += 2;
      lexBitStringLiteral(Result);
      return;
    }
    lexIdentifier(Result);
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
        skipSingleLineComment();
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
        skipMultilineComment();
        goto LexToken;
      }
    } else {
      lexCompoundDelimiter(Result);
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
    lexCompoundDelimiter(Result);
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

  formToken(Result, Kind);
}

tok::TokenKind kindOfIdentifier(StringRef Str) {
#define KEYWORD(identifier, version)                                           \
  if (Str == #identifier) {                                                    \
    return tok::kw_##identifier;                                               \
  }
#include "Common/TokenKinds.def"
  return tok::basic_identifier;
}

void Lexer::lexIdentifier(Token &Result) {
  bool wasUnderscore = false;

  while (CurrentPtr <= BufferEnd) {
    char Char = *CurrentPtr++;
    if (IsLetterDigitUnderline(Char)) {
      if (Char == '_') {
        if (wasUnderscore) {
          DiagnosticBuilder D =
              Diag->diagnose(diag::lex_consecutive_underline_identifier);
          D.setLocation(FileLocation.getLocWithOffset(
              (uint32_t)(uintptr_t)(CurrentPtr - BufferStart - 1)));
        } else {
          wasUnderscore = true;
        }
      } else {
        wasUnderscore = false;
      }
    } else {
      CurrentPtr--;
      break;
    }
  }

  formTokenWithValue(Result, tok::basic_identifier);
  Result.setKind(kindOfIdentifier(Result.getValue()));
}
void Lexer::lexExtendedIdentifier(Token &Result) { return; }

void Lexer::lexCharacterLiteral(Token &Result) {
  if (*(CurrentPtr + 1) != '\'') {
    formToken(Result, tok::tick);
    return;
  }
  CurrentPtr += 2;
  formTokenWithValue(Result, tok::character_literal);
}

void Lexer::lexStringLiteral(Token &Result) {
  bool escapeQuotation = false;
  while (true) {
    char C = *CurrentPtr++;
    if (C == '"' && !escapeQuotation) {
      if (*CurrentPtr == '"') {
        escapeQuotation = true;
      } else {
        formTokenWithValue(Result, tok::string_literal);
        return;
      }
    } else if (C == 0) {
      if (CurrentPtr != BufferEnd) {
        void(); // lexed 0 char
      } else {
        formToken(Result, tok::unknown);
        CurrentPtr--;
        return;
      }
    } else {
      escapeQuotation = false;
    }
  }
}

void Lexer::lexInteger(Token &Result) {
  SourceLocation FirstInvalid, LastInvalid;
  bool wasUnderline;

  // Check for digit as begin
  if (!IsNumeric(*CurrentPtr)) {
    // Skip additional underlines and search for last invalid underline
    FirstInvalid = FileLocation.getLocWithOffset(
        (uint32_t)(uintptr_t)(CurrentPtr - BufferStart));
    while (*(CurrentPtr++) == '_' && *CurrentPtr != 0)
      ;
    LastInvalid = FileLocation.getLocWithOffset(
        (uint32_t)(uintptr_t)(CurrentPtr - BufferStart - 1));
    DiagnosticBuilder D = Diag->diagnose(diag::lex_integer_start_underline);
    D.setLocation(FirstInvalid);
  }

  // Consume the rest of the digits and underlines
  while (IsNumericUnderline(*CurrentPtr)) {
    wasUnderline = *CurrentPtr == '_';
    CurrentPtr++;

    // Check for double underline and trailing underlines
    if (wasUnderline && *CurrentPtr == '_') {
      // Skip additional underlines and search for last invalid underline
      FirstInvalid = FileLocation.getLocWithOffset(
          (uint32_t)(uintptr_t)(CurrentPtr - BufferStart));
      while (*(CurrentPtr++) == '_' && *CurrentPtr != 0)
        ;
      LastInvalid = FileLocation.getLocWithOffset(
          (uint32_t)(uintptr_t)(CurrentPtr - BufferStart - 1));

      // Check for trailing underlines
      if (IsNumeric(*CurrentPtr)) {
        DiagnosticBuilder D =
            Diag->diagnose(diag::lex_integer_consecutive_underline);
        D.setLocation(FirstInvalid);
      } else {
        DiagnosticBuilder D =
            Diag->diagnose(diag::lex_integer_ending_underline);
        D.setLocation(FirstInvalid);
      }
    }
  }
}

void Lexer::lexNumber(Token &Result) {
  
  if (IsNumeric(*CurrentPtr))  {
    lexInteger(Result);
  }

  // Check for based literal
  if (*CurrentPtr == '#') {
    CurrentPtr++;
    lexBasedLiteral(Result);
    return;
  }

  // Check for decimal literal
  if (*CurrentPtr == '.') {
    CurrentPtr++;
    lexDecimalLiteral(Result);
    return;
  }

  // TODO: 2008 Feauture
  if (IsBitStringModifier(*CurrentPtr)) {
    CurrentPtr++;
    lexBitStringLiteral(Result);
    return;
  }

  // Every other character terminates the decimal literal
  if (IsLetter(*CurrentPtr) || (*CurrentPtr == '/' && IsLetter(*(CurrentPtr+1)))) {
    DiagnosticBuilder D =
        Diag->diagnose(diag::missing_seperator_decimal_identifier);
    D.setLocation(FileLocation.getLocWithOffset(
        (uint32_t)(uintptr_t)(CurrentPtr - BufferStart)));
  }
  formTokenWithValue(Result, tok::decimal_literal);
}

void Lexer::lexDecimalLiteral(Token &Result) {
  // Lex decimal part
  lexInteger(Result);

  // Check for exponent
  if (*CurrentPtr == 'e' || *CurrentPtr == 'E') {
    CurrentPtr++;
    // Check for sign
    if (*CurrentPtr == '-' || *CurrentPtr == '+') {
      CurrentPtr++;
    }
    // Lex exponent
    lexInteger(Result);
  }
  formTokenWithValue(Result, tok::decimal_literal);
}

void Lexer::lexBasedLiteral(Token &Result) { return; }
void Lexer::lexBitStringLiteral(Token &Result) {}

void Lexer::lexCompoundDelimiter(Token &Result) {
#define TwoByteCompound(b1, b2, t1, t2)                                        \
  if (*(CurrentPtr - 1) == b1) {                                               \
    if (*CurrentPtr == b2) {                                                   \
      ++CurrentPtr;                                                            \
      formToken(Result, t2);                                                   \
    } else {                                                                   \
      formToken(Result, t1);                                                   \
    }                                                                          \
    return;                                                                    \
  }

  TwoByteCompound('=', '>', tok::equal, tok::arrow);
  TwoByteCompound('*', '*', tok::asterisk, tok::double_star);
  TwoByteCompound(':', '=', tok::colon, tok::variable_assignment);
  TwoByteCompound('/', '=', tok::slash, tok::inequal);

  if (*(CurrentPtr - 1) == '>') {
    if (*CurrentPtr == '=') {
      CurrentPtr++;
      formToken(Result, tok::greater_equal);
    } else if (*CurrentPtr == '>') {
      CurrentPtr++;
      formToken(Result, tok::double_greater);
    } else {
      formToken(Result, tok::greater);
    }
    return;
  }
  if (*(CurrentPtr - 1) == '<') {
    if (*CurrentPtr == '=') {
      CurrentPtr++;
      formToken(Result, tok::less_equal);
    } else if (*CurrentPtr == '>') {
      CurrentPtr++;
      formToken(Result, tok::box);
    } else if (*CurrentPtr == '<') {
      CurrentPtr++;
      formToken(Result, tok::double_less);
    } else {
      formToken(Result, tok::less);
    }
    return;
  }
  if (*(CurrentPtr - 1) == '?') {
    CurrentPtr++;
    TwoByteCompound('/', '=', tok::matching_inequal, tok::matching_equal);
    TwoByteCompound('<', '=', tok::matching_less_equal, tok::matching_less);
    TwoByteCompound('>', '=', tok::matching_greate_equal,
                    tok::matching_greater);
    if (*(CurrentPtr - 1) == '?') {
      formToken(Result, tok::condition_conversion);
    } else {
      CurrentPtr--; // Set pointer back since only single byte delimiter
      formToken(Result, tok::question_mark);
    }
    return;
  }
}

void Lexer::lexSingleLineComment(Token &Result) { return; }
void Lexer::lexMultiLineComment(Token &Result) { return; }

void Lexer::skipSingleLineComment() {}
void Lexer::skipMultilineComment() {}
void Lexer::skipWhitespace() {
  bool SawNewline = false;

  while (true) {
    // skip all spaces
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
  TokenStart = CurrentPtr;
}
