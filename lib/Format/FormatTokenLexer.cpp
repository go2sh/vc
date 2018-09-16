#include "FormatTokenLexer.h"
using namespace vc::format;

// void FormatTokenLexer::reset() {

// }

void FormatTokenLexer::lex(std::vector<FormatToken *> &Tokens) {
  do {
    Tokens.push_back(getFormatToken());
  } while (Tokens.back()->Tok.isNot(tok::eof));
}

FormatToken *FormatTokenLexer::getFormatToken() {
  FormatToken *FT = new FormatToken();
  L.lex(FT->Tok);
  FT->TokenText = FT->Tok.getValue();

  SourceLocation WhitespaceStart =
      FT->Tok.getLocation().getLocWithOffset(-TrailingWhitespaces);

  unsigned WhitespaceLength = 0;
  TrailingWhitespaces = 0;
  while (FT->Tok.is(tok::unknown)) {
    StringRef TokenText = FT->Tok.getValue();
    for (std::size_t i = 0; i < TokenText.length(); i++) {
      switch (TokenText[i]) {
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
      case 0xA0:
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
  FT->Column = Column;
  FT->WhitespaceRange = SourceRange(
      WhitespaceStart, WhitespaceStart.getLocWithOffset(WhitespaceLength));

  Column += FT->TokenText.length();
  
  if (FT->Tok.is(tok::comment)) {
    StringRef UntrimmedText = FT->TokenText;
    FT->TokenText = UntrimmedText.rtrim(" \xA0\t\f\v");
    TrailingWhitespaces += UntrimmedText.size() - FT->TokenText.size();
  }

  return FT;
}