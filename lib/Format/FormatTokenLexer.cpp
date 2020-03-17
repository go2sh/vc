#include "FormatTokenLexer.h"
using namespace vc::format;

// void FormatTokenLexer::reset() {

// }

void FormatTokenLexer::lex() {
  do {
    Tokens.push_back(getFormatToken());
  } while (Tokens.back()->Tok.isNot(tok::eof));
}

auto FormatTokenLexer::getFormatToken() -> FormatToken * {
  auto *FT = new FormatToken(); // NOLINT(cppcoreguidelines-owning-memory)
  L.lex(FT->Tok);

  SourceLocation WhitespaceStart =
      FT->Tok.getLocation().getLocWithOffset(-TrailingWhitespaces);
  unsigned WhitespaceLength = 0;
  TrailingWhitespaces = 0;

  while (FT->Tok.is(tok::unknown)) {
    StringRef TokenText = FT->Tok.getValue();
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
  FT->WhitespacePrefix = 
      StringRef(FT->Tok.getValue().data() - WhitespaceLength, WhitespaceLength);
  FT->WhitespaceRange = SourceRange(
      WhitespaceStart, WhitespaceStart.getLocWithOffset(WhitespaceLength));

  // FIXME: Multi-Byte chars break columns
  FT->ColumnWidth = FT->TokenText.length();
  Column += FT->ColumnWidth;

  if (FT->Tok.is(tok::comment)) {
    StringRef UntrimmedText = FT->TokenText;
    FT->TokenText = UntrimmedText.rtrim(" \xA0\t\f\v");
    TrailingWhitespaces += UntrimmedText.size() - FT->TokenText.size();
  }

  return FT;
}