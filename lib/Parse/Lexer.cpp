#include <cassert>
#include <iostream>

#include "Common/CharInfo.h"
#include "Common/TokenKinds.h"
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

Lexer::Lexer(const char *Buffer, int Length) {
  BufferStart = Buffer;
  BufferEnd = Buffer + Length;
  BufferPtr = (char *)Buffer;
  isAtNewline = true;
  hasWhitespacePrefix = false;
}

void Lexer::lex(Token &Result) {
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
  case 0:
    if (CurrentPtr - 1 != BufferEnd) {
      //
      goto LexToken;
    }
    CurrentPtr--;
    formToken(Result, tok::eof, CurrentPtr);
    return;
  case ' ':
  case (char)0xA0: // NBSP
  case '\t':
    hasWhitespacePrefix = true;
    skipWhitespace(CurrentPtr);
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
    skipWhitespace(CurrentPtr);
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
    lexNumber(Result, CurrentPtr);
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
    lexIdentifier(Result, CurrentPtr);
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
    Char = *CurrentPtr;
    if (Char == '"') {
      // We use the BufferPtr to get in sync with a preceeding length
      lexBitStringLiteral(Result, BufferPtr);
      return;
    }
    lexIdentifier(Result, CurrentPtr);
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
      // Get in sync with preeciding length
      lexBitStringLiteral(Result, BufferPtr);
      return;
    }
    lexIdentifier(Result, CurrentPtr);
    return;

  case '\'':
    lexCharacterLiteral(Result, CurrentPtr);
    return;
  case '"':
    lexStringLiteral(Result, CurrentPtr);
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
      lexCompoundDelimiter(Result, CurrentPtr);
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
    lexCompoundDelimiter(Result, CurrentPtr);
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

  formToken(Result, Kind, CurrentPtr);
}

tok::TokenKind kindOfIdentifier(std::string & Str) {
  #define KEYWORD(identifier, version) if (Str == #identifier) {return tok::kw_##identifier;}
  #include "Common/TokenKinds.def"
  return tok::basic_identifier;
}

void Lexer::lexIdentifier(Token &Result, const char *CurrentPtr) {
  bool wasUnderscore = false;

  while (CurrentPtr <= BufferEnd) {
    char Char = *CurrentPtr++;
    if (IsLetterDigitUnderline(Char)) {
      if (Char == '_') {
        if (wasUnderscore) {
          cout << "Adjacent underscores not allowed.";
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

  formTokenWithValue(Result, tok::basic_identifier, CurrentPtr);
  Result.setKind(kindOfIdentifier(Result.getValue()));
}
void Lexer::lexExtendedIdentifier(Token &Result) { return; }

void Lexer::lexCharacterLiteral(Token &Result, const char *CurrentPtr) {
  if (*(CurrentPtr + 1) != '\'') {
    formToken(Result, tok::tick, CurrentPtr);
    return;
  }
  CurrentPtr += 2;
  formTokenWithValue(Result, tok::character_literal, CurrentPtr);
}

void Lexer::lexStringLiteral(Token &Result, const char * CurrentPtr) {
  bool escapeQuotation = false;
  while (true) {
    char C = *CurrentPtr++;
    if (C == '"' && !escapeQuotation) {
      if (*CurrentPtr == '"') {
        escapeQuotation = true;
      } else {
        formTokenWithValue(Result, tok::string_literal, CurrentPtr);
        return;
      }
    } else if (C == 0) {
      if (CurrentPtr != BufferEnd) {
        void(); // lexed 0 char
      } else {
        formToken(Result, tok::unknown, CurrentPtr);
        CurrentPtr--;
        return;
      }
    }else {
      escapeQuotation = false;
    }
  }
}

void Lexer::lexNumber(Token &Result, const char *CurrentPtr) {
  bool wasUnderline = false;

  // Consume base or integer
  while (IsNumericUnderline(*CurrentPtr)) {
    if (wasUnderline && *CurrentPtr == '_') {
      cout << "Error: Consecutive underline not allowed." << endl;
    }
    wasUnderline = *CurrentPtr == '_';
    CurrentPtr++;
  }

  // Check for based literal
  if (*CurrentPtr == '#') {
    lexBasedLiteral(Result, CurrentPtr++);
    return;
  }

  // Check for decimal literal
  if (*CurrentPtr == '.') {
    lexDecimalLiteral(Result, CurrentPtr++);
  }

  if (IsBitStringModifier(*CurrentPtr)) {
    // We have a bit string literal
    if (*CurrentPtr == 's' || *CurrentPtr == 'S' || *CurrentPtr == 'u' ||
        *CurrentPtr == 'U') {
      if (IsBitStringBase(*(++CurrentPtr))) {
        lexBitStringLiteral(Result, CurrentPtr++);
      } else {
        std::cout << "Unexspected character." << endl;
      }
    } else if (IsBitStringBase(*CurrentPtr)) {
      lexBitStringLiteral(Result, CurrentPtr++);
    } else {
      std::cout << "Unexspected character." << endl;
    }
    return;
  } else if (!IsLetter(*CurrentPtr)) {
    // Every other character then a letter terminates the decimal literal
    formTokenWithValue(Result, tok::decimal_literal, CurrentPtr);
  } else {
    // seperator between literals and identifier
    std::cout << "Need seperator between identifier and decimal literal."
              << endl;
  }
}

void Lexer::lexDecimalLiteral(Token &Result, const char *CurrentPtr) {
  bool wasUnderline = false;

  if (!IsNumeric(*CurrentPtr)) {
    cout << "Expected Numeric character." << endl;
  }
  while (IsNumericUnderline(*CurrentPtr)) {
    if (wasUnderline && *CurrentPtr == '_') {
      cout << "Error: Consecutive underline not allowed." << endl;
    }
    wasUnderline = *CurrentPtr == '_';
    CurrentPtr++;
  }

  // Check for exponent
  if (*CurrentPtr == 'e' || *CurrentPtr == 'E') {
    CurrentPtr++;
    // Check for sign
    if (*CurrentPtr == '-' || *CurrentPtr == '+') {
      CurrentPtr++;
    }
    if (!IsNumeric(*CurrentPtr)) {
      cout << "Expected Numeric character." << endl;
    }
    while (IsNumericUnderline(*CurrentPtr)) {
      if (wasUnderline && *CurrentPtr == '_') {
        cout << "Error: Consecutive underline not allowed." << endl;
      }
      wasUnderline = *CurrentPtr == '_';
      CurrentPtr++;
    }
  }
  formTokenWithValue(Result, tok::decimal_literal, CurrentPtr);
}
void Lexer::lexBasedLiteral(Token &Result, const char *CurrentPtr) { return; }
void Lexer::lexBitStringLiteral(Token &Result, const char *CurrentPtr) {}

void Lexer::lexCompoundDelimiter(Token &Result, const char *CurrentPtr) {
#define TwoByteCompound(b1, b2, t1, t2)                                        \
  if (*(CurrentPtr - 1) == b1) {                                               \
    if (*CurrentPtr == b2) {                                                   \
      ++CurrentPtr;                                                            \
      formToken(Result, t2, CurrentPtr);                                       \
    } else {                                                                   \
      formToken(Result, t1, CurrentPtr);                                       \
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
      formToken(Result, tok::greater_equal, CurrentPtr);
    } else if (*CurrentPtr == '>') {
      CurrentPtr++;
      formToken(Result, tok::double_greater, CurrentPtr);
    } else {
      formToken(Result, tok::greater, CurrentPtr);
    }
    BufferPtr = CurrentPtr;
    return;
  }
  if (*(CurrentPtr - 1) == '<') {
    if (*CurrentPtr == '=') {
      CurrentPtr++;
      formToken(Result, tok::less_equal, CurrentPtr);
    } else if (*CurrentPtr == '>') {
      CurrentPtr++;
      formToken(Result, tok::box, CurrentPtr);
    } else if (*CurrentPtr == '<') {
      CurrentPtr++;
      formToken(Result, tok::double_less, CurrentPtr);
    } else {
      formToken(Result, tok::less, CurrentPtr);
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
      formToken(Result, tok::condition_conversion, CurrentPtr);
    } else {
      CurrentPtr--; // Set pointer back since only single byte delimiter
      formToken(Result, tok::question_mark, CurrentPtr);
    }
    return;
  }
}

void Lexer::lexSingleLineComment(Token &Result) { return; }
void Lexer::lexMultiLineComment(Token &Result) { return; }

void Lexer::skipSingleLineComment() {}
void Lexer::skipMultilineComment() {}
void Lexer::skipWhitespace(const char *CurrentPtr) {
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
  BufferPtr = CurrentPtr;
}
