#include "FormatTokenLexer.h"
using namespace vc::format;

FormatTokenLexer::~FormatTokenLexer() {
  for (auto *Token : Tokens) {
    delete Token;
  }
  Tokens.clear();
}

void FormatTokenLexer::lex() {
  do {
    Tokens.push_back(getFormatToken());
  } while (Tokens.back()->Tok.isNot(TokenKind::eof));
}

auto FormatTokenLexer::getFormatToken() -> FormatToken * {
  auto *FT = new FormatToken(); // NOLINT(cppcoreguidelines-owning-memory)
  L.lex(FT->Tok);

  SourceLocation WhitespaceStart =
      FT->Tok.getLocation().getLocWithOffset(-TrailingWhitespaces);
  unsigned WhitespaceLength = 0;
  TrailingWhitespaces = 0;

  while (FT->Tok.is(TokenKind::unknown)) {
    std::string_view TokenText = FT->Tok.getValue();
    for (const char C : TokenText) {
      switch (C) {
      case '\n':
        FT->NewLinesBefore++;
        Column = 0;
        break;
      case '\r':
      case '\f':
      case '\v':
        Column = 0;
        break;
      case ' ':
      case '\xA0':
        Column++;
        break;
      case '\t':
        Column += Style.TabSize;
        break;
      default:
        break;
      }
    }

    WhitespaceLength += FT->Tok.getValue().size();
    L.lex(FT->Tok);
  }

  FT->TokenText = FT->Tok.getValue();
  FT->WhitespacePrefix = std::string_view(
      FT->Tok.getValue().data() - WhitespaceLength, WhitespaceLength);
  FT->WhitespaceRange = SourceRange(
      WhitespaceStart, WhitespaceStart.getLocWithOffset(WhitespaceLength));

  // FIXME: Multi-Byte chars break columns
  FT->ColumnWidth = FT->TokenText.length();
  Column += FT->ColumnWidth;

  if (FT->Tok.is(TokenKind::comment)) {
    std::string_view UntrimmedText = FT->TokenText;
    UntrimmedText.remove_suffix(std::min(
        UntrimmedText.find_last_not_of(" \xA0\t\f\v"), UntrimmedText.size()));
    TrailingWhitespaces += UntrimmedText.size() - FT->TokenText.size();
    FT->TokenText = UntrimmedText;
  }

  return FT;
}