#include <cassert>
#include <iostream>

#include "Common/CharInfo.h"

#include "Parse/DiagnosticsLex.h"
#include "Parse/Lexer.h"
#include "Parse/Token.h"

using namespace vc;

Lexer::Lexer(SourceLocation FileLocation, const MemoryBuffer *Buffer)
    : FileLocation(FileLocation) {
  BufferStart = Buffer->getStart();
  BufferEnd = Buffer->getEnd();
  TokenStart = BufferStart;
  CurrentPtr = BufferStart;
  isAtNewline = true;
  hasWhitespacePrefix = false;
  RawMode = true;
}

Lexer::Lexer(SourceLocation FileLocation, const MemoryBuffer *Buffer,
             DiagnosticEngine &Diag)
    : Diag(&Diag), FileLocation(FileLocation) {
  BufferStart = Buffer->getStart();
  BufferEnd = Buffer->getEnd();
  TokenStart = BufferStart;
  CurrentPtr = BufferStart;
  isAtNewline = true;
  hasWhitespacePrefix = false;
}

void Lexer::lex(Token &Result) {
  if (NextToken.isNot(TokenKind::eof)) {
    lexToken();
  }
  Result = NextToken;
}

void Lexer::lexToken() {
LexToken:
  char Char = *(CurrentPtr++);

  switch (Char) {
  case 0:
    if (CurrentPtr - 1 != BufferEnd) {
      // TODO: Error null byte in char stream
      goto LexToken;
    }
    CurrentPtr--;
    return formToken(TokenKind::eof);

  case ' ':
  case (char)0xA0: // NBSP
  case '\t':
    if (skipWhitespace())
      return;
    goto LexToken;

  case '\v':
  case '\f':
  case '\r':
    if (*CurrentPtr == '\n') {
      Char = *(CurrentPtr++);
    }
    [[fallthrough]];
  case '\n':
    if (skipWhitespace())
      return;
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
    return lexNumber();

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
    return lexIdentifier();

  // Simple bit string literal chars
  case 'b':
  case 'B':
  case 'o':
  case 'O':
  case 'x':
  case 'X':
  case 'd':
  case 'D':
    if (*CurrentPtr != '"') {
      return lexIdentifier();
    }
    CurrentPtr++;
    return lexBitStringLiteral();

  // Bitstring modifier chars
  case 'u':
  case 'U':
  case 's':
  case 'S':
    if (!IsBitStringBase(*CurrentPtr) || *(CurrentPtr + 1) != '"') {
      return lexIdentifier();
    }
    CurrentPtr += 2;
    return lexBitStringLiteral();

  case '\'':
    return lexCharacterLiteral();
  case '"':
    return lexStringLiteral();
  case '\\':
    return lexExtendedIdentifier();
  case '-':
    if (*CurrentPtr == '-') {
      // Return if we keep the comment token
      if (skipSingleLineComment()) {
        return;
      }
      goto LexToken;
    }

    return formToken(TokenKind::minus);

  case '/':
    if (*CurrentPtr == '*') {
      if (skipMultilineComment()) {
        return;
      }
      goto LexToken;
    }
    return lexCompoundDelimiter();

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
    return lexCompoundDelimiter();

  // Normal delimiters
  case '&':
    return formToken(TokenKind::ampersand);
  case '(':
    return formToken(TokenKind::left_parenthesis);
  case ')':
    return formToken(TokenKind::right_parenthesis);
  case '+':
    return formToken(TokenKind::plus);
  case ',':
    return formToken(TokenKind::colon);
  case '.':
    return formToken(TokenKind::dot);
  case ';':
    return formToken(TokenKind::semicolon);
  case '`':
    return formToken(TokenKind::grave);
  case '|':
    return formToken(TokenKind::vertical_line);
  case '[':
    return formToken(TokenKind::left_square);
  case ']':
    return formToken(TokenKind::right_square);
  case '@':
    return formToken(TokenKind::at);
  default:
    // Default is an invalid token, maybe add gracefull error handlings for UTF8
    // Chars
    return formToken(TokenKind::unknown);
  }
}

static constexpr auto kindOfIdentifier(std::string_view Str) -> TokenKind {
#define KEYWORD(identifier, version)                                           \
  if (Str == #identifier) {                                                    \
    return TokenKind::kw_##identifier;                                         \
  }
#include "Parse/TokenKinds.def"
  return TokenKind::basic_identifier;
}

void Lexer::lexIdentifier() {
  bool wasUnderscore = false;

  while (CurrentPtr <= BufferEnd) {
    char Char = *CurrentPtr;
    if (IsLetterDigitUnderline(Char)) {
      if (Char == '_') {
        if (wasUnderscore) {
          if (!RawMode) {
            DiagnosticBuilder D =
                Diag->diagnose(diag::lex_consecutive_underline_identifier);
            D.setLocation(FileLocation.getLocWithOffset(
                (uint32_t)(uintptr_t)(CurrentPtr - BufferStart - 1)));
          }
        } else {
          wasUnderscore = true;
        }
      } else {
        wasUnderscore = false;
      }
      CurrentPtr++;
    } else {
      break;
    }
  }

  formToken(TokenKind::basic_identifier);
  formToken(kindOfIdentifier(NextToken.getValue()));
}
void Lexer::lexExtendedIdentifier() { return; }

void Lexer::lexCharacterLiteral() {
  if (*(CurrentPtr + 1) != '\'') {
    return formToken(TokenKind::tick);
  }
  CurrentPtr += 2;
  formToken(TokenKind::character_literal);
}

void Lexer::lexStringLiteral() {
  bool escapeQuotation = false;
  while (true) {
    char C = *CurrentPtr++;
    if (C == '"' && !escapeQuotation) {
      if (*CurrentPtr == '"') {
        escapeQuotation = true;
      } else {
        formToken(TokenKind::string_literal);
        return;
      }
    } else if (C == 0) {
      if (CurrentPtr != BufferEnd) {
        void(); // lexed 0 char
      } else {
        formToken(TokenKind::unknown);
        CurrentPtr--;
        return;
      }
    } else {
      escapeQuotation = false;
    }
  }
}

void Lexer::lexInteger() {
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
    if (!RawMode) {
      DiagnosticBuilder D = Diag->diagnose(diag::lex_integer_start_underline);
      D.setLocation(FirstInvalid);
    }
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
        if (!RawMode) {
          DiagnosticBuilder D =
              Diag->diagnose(diag::lex_integer_consecutive_underline);
          D.setLocation(FirstInvalid);
        }
      } else {
        if (!RawMode) {
          DiagnosticBuilder D =
              Diag->diagnose(diag::lex_integer_ending_underline);
          D.setLocation(FirstInvalid);
        }
      }
    }
  }
}

void Lexer::lexNumber() {

  if (IsNumeric(*CurrentPtr)) {
    lexInteger();
  }

  // Check for based literal
  if (*CurrentPtr == '#') {
    CurrentPtr++;
    lexBasedLiteral();
    return;
  }

  // Check for decimal literal
  if (*CurrentPtr == '.') {
    CurrentPtr++;
    lexDecimalLiteral();
    return;
  }

  // TODO: 2008 Feauture
  if (IsBitStringModifier(*CurrentPtr)) {
    CurrentPtr++;
    lexBitStringLiteral();
    return;
  }

  // Every other character terminates the decimal literal
  if (IsLetter(*CurrentPtr) ||
      (*CurrentPtr == '/' && IsLetter(*(CurrentPtr + 1)))) {
    if (!RawMode) {
      DiagnosticBuilder D =
          Diag->diagnose(diag::missing_seperator_decimal_identifier);
      D.setLocation(FileLocation.getLocWithOffset(
          (uint32_t)(uintptr_t)(CurrentPtr - BufferStart)));
    }
  }
  formToken(TokenKind::decimal_literal);
}

void Lexer::lexDecimalLiteral() {
  // Lex decimal part
  lexInteger();

  // Check for exponent
  if (*CurrentPtr == 'e' || *CurrentPtr == 'E') {
    CurrentPtr++;
    // Check for sign
    if (*CurrentPtr == '-' || *CurrentPtr == '+') {
      CurrentPtr++;
    }
    // Lex exponent
    lexInteger();
  }
  formToken(TokenKind::decimal_literal);
}

void Lexer::lexBasedLiteral() { return; }
void Lexer::lexBitStringLiteral() {}

void Lexer::lexCompoundDelimiter() {
#define TwoByteCompound(b1, b2, t1, t2)                                        \
  if (*(CurrentPtr - 1) == b1) {                                               \
    if (*CurrentPtr == b2) {                                                   \
      ++CurrentPtr;                                                            \
      formToken(t2);                                                           \
    } else {                                                                   \
      formToken(t1);                                                           \
    }                                                                          \
    return;                                                                    \
  }

  TwoByteCompound('=', '>', TokenKind::equal, TokenKind::arrow);
  TwoByteCompound('*', '*', TokenKind::asterisk, TokenKind::double_star);
  TwoByteCompound(':', '=', TokenKind::colon, TokenKind::variable_assignment);
  TwoByteCompound('/', '=', TokenKind::slash, TokenKind::inequal);

  if (*(CurrentPtr - 1) == '>') {
    if (*CurrentPtr == '=') {
      CurrentPtr++;
      formToken(TokenKind::greater_equal);
    } else if (*CurrentPtr == '>') {
      CurrentPtr++;
      formToken(TokenKind::double_greater);
    } else {
      formToken(TokenKind::greater);
    }
    return;
  }
  if (*(CurrentPtr - 1) == '<') {
    if (*CurrentPtr == '=') {
      CurrentPtr++;
      formToken(TokenKind::less_equal);
    } else if (*CurrentPtr == '>') {
      CurrentPtr++;
      formToken(TokenKind::box);
    } else if (*CurrentPtr == '<') {
      CurrentPtr++;
      formToken(TokenKind::double_less);
    } else {
      formToken(TokenKind::less);
    }
    return;
  }
  if (*(CurrentPtr - 1) == '?') {
    CurrentPtr++;
    TwoByteCompound('/', '=', TokenKind::matching_inequal,
                    TokenKind::matching_equal);
    TwoByteCompound('<', '=', TokenKind::matching_less_equal,
                    TokenKind::matching_less);
    TwoByteCompound('>', '=', TokenKind::matching_greater_equal,
                    TokenKind::matching_greater);
    if (*(CurrentPtr - 1) == '?') {
      formToken(TokenKind::condition_conversion);
    } else {
      CurrentPtr--; // Set pointer back since only single byte delimiter
      formToken(TokenKind::question_mark);
    }
    return;
  }
}

bool Lexer::skipSingleLineComment() {
  // Skip the second '-'
  CurrentPtr++;

  while (true) {
    if (IsHorizontalWhitespace(*CurrentPtr)) {
      // Read \r\n as once
      if (*CurrentPtr == '\r' && *(CurrentPtr + 1) == '\n') {
        CurrentPtr += 2;
      } else {
        CurrentPtr++;
      }
      break;
    }
    // TODO: Check eof

    CurrentPtr++;
  }

  if (KeepComments) {
    formToken(TokenKind::comment);
    return true;
  }

  return false;
}

bool Lexer::skipMultilineComment() {
  // Skip the '*' from the beginning
  CurrentPtr++;

  while (true) {
    if (*CurrentPtr == '\0') {
      if (!RawMode) {
        // TODO: Diag no end
      }
      break;
    }
    if (*CurrentPtr == '*' && *(CurrentPtr + 1) == '/') {
      CurrentPtr += 2;
      break;
    }
    CurrentPtr++;
  }

  if (KeepComments) {
    formToken(TokenKind::comment);
    return true;
  }

  return false;
}

bool Lexer::skipWhitespace() {
  bool SawNewline = false;

  // skip all spaces
  while (IsHorizontalWhitespace(*CurrentPtr) ||
         IsVerticalWhitespace(*CurrentPtr)) {
    if (IsVerticalWhitespace(*CurrentPtr)) {
      SawNewline = true;
    }
    CurrentPtr++;
  }

  if (SawNewline) {
    isAtNewline = true;
    hasWhitespacePrefix = false;
    if (IsHorizontalWhitespace(*(CurrentPtr - 1))) {
      hasWhitespacePrefix = true;
    }
  } else {
    hasWhitespacePrefix = true;
  }

  if (KeepWhitespaces) {
    formToken(TokenKind::unknown);
    return true;
  }

  return false;
}
